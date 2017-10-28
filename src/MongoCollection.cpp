#include "MongoCollection.h"
#include "MongoDB.h"

MongoCollection::MongoCollection(MongoDB *pMongoDB, const String name, mongoc_collection_t *pCollection) {
	_pMongoDB = pMongoDB;
	_name = name;
	_pCollection = pCollection;
}

MongoCollection::~MongoCollection() {
	mongoc_collection_destroy(_pCollection);
}

String MongoCollection::getName() const {
	return _name;
}

String MongoCollection::getFullName() const {
	return _pMongoDB->getName() + "." + _name;
}

MongoDB* MongoCollection::getDatabase() const {
	return _pMongoDB;
}

mongoc_cursor_t* MongoCollection::find(const bson_t *pQuery, const bson_t *pProjection) const {
	bson_t *pOptions;
	if (pProjection) {
		pOptions = bson_new();
		bson_append_document(pOptions, "projection", 10, pProjection);
	}
	else {
		pOptions = nullptr;
	}
	mongoc_cursor_t* pCursor = mongoc_collection_find_with_opts(_pCollection, pQuery, pOptions, NULL);
	if (pOptions) {
		bson_destroy(pOptions);
	}
	return pCursor;
}

mongoc_cursor_t* MongoCollection::aggregate(const bson_t *pPipeline, const bson_t *pOptions) const {
	return  mongoc_collection_aggregate(_pCollection, MONGOC_QUERY_NONE, pPipeline, pOptions, NULL);
}

const int64_t MongoCollection::count(const bson_t *pQuery, bson_error_t *pError) const {
	return mongoc_collection_count(_pCollection, MONGOC_QUERY_NONE, pQuery, 0, 0, NULL, pError);
}

const bool MongoCollection::distinct(const String key, const bson_t *pQuery, bson_t *pReply, bson_error_t *pError) const {
	bson_t *pCommand = bson_new();
	bson_append_utf8(pCommand, "distinct", 8, _name.c_str(), (int)_name.size());
	bson_append_utf8(pCommand, "key", 3, key.c_str(), (int)key.size());
	bson_append_document(pCommand, "query", 5, pQuery);
	bool status = mongoc_database_command_simple(_pMongoDB->getMongo(), pCommand, NULL, pReply, pError);
	bson_destroy(pCommand);
	return status;
}

const bool MongoCollection::insert(const bson_t *pDoc, bson_t *pWR, bson_error_t *pError) const {
	if (mongoc_collection_insert(_pCollection, MONGOC_INSERT_NONE, pDoc, NULL, pError)) {
		if (pWR) {
			bson_append_double(pWR, "nInserted", 9, 1);
		}
		return true;
	}
	return false;
}

const bool MongoCollection::save(const bson_t *pDoc, bson_t *pWR, bson_error_t *pError) const {
	bson_iter_t it;
	//Upsert
	if (bson_iter_init_find(&it, pDoc, "_id")) {
		bson_t *pQuery = bson_new();
		switch (bson_iter_type(&it)) {
		case BSON_TYPE_OID: 
			bson_append_oid(pQuery, "_id", 3, bson_iter_oid(&it));
			break;
		case BSON_TYPE_UTF8: {
			uint32_t length;
			const char *idStr = bson_iter_utf8(&it, &length);
			bson_append_utf8(pQuery, "_id", 3, idStr, length);
		}
			break;
		}
		bool ok = mongoc_collection_update(_pCollection, MONGOC_UPDATE_UPSERT, pQuery, pDoc, NULL, pError);
		bson_destroy(pQuery);
		return ok;
	}
	//Insert
	else {
		return insert(pDoc, pWR, pError);
	}
	return true;
}

const bool MongoCollection::update(const bson_t *pQuery, const bson_t *pUpdate, const bson_t *pOptions, bson_t *pWR, bson_error_t *pError) const {
	bson_t arr, doc, *pCommand = bson_new();
	bson_append_utf8(pCommand, "update", 6, _name.c_str(), (int)_name.size());
	bson_append_array_begin(pCommand, "updates", 7, &arr);
	bson_append_document_begin(&arr, "0", 1, &doc);
	bson_append_document(&doc, "q", 1, pQuery);
	bson_append_document(&doc, "u", 1, pUpdate);

	const char *str = bson_as_json(pUpdate, 0);

	if (pOptions) {
		bson_iter_t it;
		if (bson_iter_init(&it, pOptions)) {
			while (bson_iter_next(&it)) {
				const char *key = bson_iter_key(&it);
				if (!strcmp(key, "upsert")) {
					bson_append_bool(&doc, "upsert", 6, bson_iter_bool(&it));
				}
				if (!strcmp(key, "multi")) {
					bson_append_bool(&doc, "multi", 5, bson_iter_bool(&it));
				}
			}
		}
	}
	bson_append_document_end(&arr, &doc);
	bson_append_array_end(pCommand, &arr);

	bson_t reply;
	bool ok = mongoc_collection_command_simple(_pCollection, pCommand, nullptr, &reply, pError);
	if (ok && pWR) {
		bson_iter_t it;
		if (bson_iter_init(&it, &reply)) {
			if (bson_iter_find_descendant(&it, "upserted.0._id", &it)) {
				bson_append_double(pWR, "nMatched", 8, 0);
				bson_append_double(pWR, "nUpserted", 9, 1);
				bson_append_double(pWR, "nModified", 9, 0);
				const bson_oid_t *pOid = bson_iter_oid(&it);
				if (pOid) {
					bson_append_oid(pWR, "_id", 3, pOid);
				}
			}
			else {
				bson_append_double(pWR, "nMatched", 8, bson_iter_init_find(&it, &reply, "n") ? bson_iter_int32(&it) : 0);
				bson_append_double(pWR, "nUpserted", 9, 0);
				bson_append_double(pWR, "nModified", 9, bson_iter_init_find(&it, &reply, "nModified") ? bson_iter_int32(&it) : 0);
			}
		}
	}

	bson_destroy(pCommand);
	bson_destroy(&reply);
	return ok;
}

const bool MongoCollection::remove(const bson_t *pQuery, bool justOne, bson_t *pWR, bson_error_t *pError) const {
	bson_t *pCommand = bson_new();
	bson_append_utf8(pCommand, "delete", 6, _name.c_str(), (int)_name.size());
	bson_t arr, doc;
	bson_append_array_begin(pCommand, "deletes", 7, &arr);
	bson_append_document_begin(&arr, "0", 1, &doc);
	bson_append_document(&doc, "q", 1, pQuery);
	bson_append_int32(&doc, "limit", 5, justOne ? 1 : 0);
	bson_append_document_end(&arr, &doc);
	bson_append_array_end(pCommand, &arr);
	
	bson_t reply;
	bool ok = mongoc_collection_command_simple(_pCollection, pCommand, nullptr, &reply, pError);
	if (ok && pWR) {
		bson_iter_t it;
		if (bson_iter_init_find(&it, &reply, "n")) {
			bson_append_double(pWR, "nRemoved", 8, bson_iter_int32(&it));
		}
	}
	
	bson_destroy(pCommand);
	bson_destroy(&reply);
	return ok;
}

const bool MongoCollection::renameCollection(const char *newName, const bool dropExisting, bson_error_t *pError) const {
	return mongoc_collection_rename(_pCollection, _pMongoDB->getName().c_str(), newName, dropExisting, pError);
}

const bool MongoCollection::runCommand(const bson_t *pCommand, bson_t *pReply, bson_error_t *pError) const {
	return mongoc_collection_command_simple(_pCollection, pCommand, nullptr, pReply, pError);
}

mongoc_bulk_operation_t* MongoCollection::createBulkOperation(const bool ordered) const {
	return mongoc_collection_create_bulk_operation(_pCollection, ordered, NULL);
}

const bool MongoCollection::executeBulkOperation(mongoc_bulk_operation_t* pBulk, bson_t *pReply, bson_error_t *pError) const {
	bool ok = mongoc_bulk_operation_execute(pBulk, pReply, pError);
	mongoc_bulk_operation_destroy(pBulk);
	return ok;
}

const bool MongoCollection::drop(bson_error_t *pError) const {
	return mongoc_collection_drop(_pCollection, pError);
}

const bool MongoCollection::createIndex(bson_t *pKeys, bson_t *pOptions, bson_t *pReply, bson_error_t *pError) const {
	char *pName = nullptr;
	bson_t indexes, *pCommand = bson_new();
	bson_append_utf8(pCommand, "createIndexes", 13, _name.c_str(), (int)_name.size());
	bson_append_array_begin(pCommand, "indexes", 7, &indexes);
	//Use given options
	if (pOptions) {
		bson_iter_t it;
		bson_append_document(pOptions, "key", 3, pKeys);
		//Name is missing
		if (!bson_iter_init_find(&it, pOptions, "name")) {
			pName = mongoc_collection_keys_to_index_string(pKeys);
			bson_append_utf8(pOptions, "name", 4, pName, -1);
		}
		bson_append_document(&indexes, "0", 1, pOptions);
	}
	//Create new options
	else {
		bson_t options;
		bson_append_document_begin(&indexes, "0", 1, &options);
		bson_append_document(&options, "key", 3, pKeys);
		pName = mongoc_collection_keys_to_index_string(pKeys);
		bson_append_utf8(&options, "name", 4, pName, -1);
		bson_append_document_end(&indexes, &options);
	}
	bson_append_array_end(pCommand, &indexes);

	bool ok = mongoc_database_write_command_with_opts(_pMongoDB->getMongo(), pCommand, nullptr, pReply, pError);

	bson_destroy(pCommand);
	if (pName) {
		bson_free(pName);
	}
	return ok;
}

const bool MongoCollection::dropIndex(const char *pName, bson_error_t *pError) const {
	return mongoc_collection_drop_index_with_opts(_pCollection, pName, nullptr, pError);
}