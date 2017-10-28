#include "MongoDB.h"

MongoDB::MongoDB(mongoc_client_t *pClient, const String name) {
	_pClient = pClient;
	_name = name;
	_pDatabase = mongoc_client_get_database(pClient, name.c_str());
}

MongoDB::~MongoDB() {
	for (auto it : _collections) {
		delete it.second;
	}
	mongoc_database_destroy(_pDatabase);
}

String MongoDB::getName() const {
	return _name;
}

mongoc_database_t* MongoDB::getMongo() const {
	return _pDatabase;
}

MongoCollection* MongoDB::getCollection(const String name) {
	std::unordered_map<String, MongoCollection*>::const_iterator found = _collections.find(name);

	//Already have this collection.
	if (found != _collections.end()) {
		return found->second;
	}

	//Create new collection
	mongoc_collection_t *p = mongoc_database_get_collection(_pDatabase, name.c_str());
	MongoCollection *coll = new MongoCollection(this, name, p);
	_collections[name] = coll;
	return coll;
}

MongoCollection* MongoDB::createCollection(const String name, const bson_t *pOptions, bson_error_t *pError) {
	mongoc_collection_t *p = mongoc_database_create_collection(_pDatabase, name.c_str(), pOptions, pError);
	if (p) {
		MongoCollection *coll = new MongoCollection(this, name, p);
		_collections[name] = coll;
		return coll;
	}
	return nullptr;
}

const bool MongoDB::hasCollection(const char *pName, bson_error_t *pError) const {
	return mongoc_database_has_collection(_pDatabase, pName, pError);
}

const bool MongoDB::getCollectionNames(std::vector<String> &namesOut, bson_error_t *pError) const {
	char **strv;
	if ((strv = mongoc_database_get_collection_names(_pDatabase, pError))) {
		for (int i = 0; strv[i]; i++) {
			namesOut.push_back(strv[i]);
		}
	}
	bson_strfreev(strv);
	return !!strv;
}

const bool MongoDB::runCommand(const bson_t *pCommand, bson_t *pReply, bson_error_t *pError) const {
	return mongoc_database_command_simple(_pDatabase, pCommand, nullptr, pReply, pError);
}

const bool MongoDB::drop(bson_error_t *pError) const {
	return mongoc_database_drop_with_opts(_pDatabase, nullptr, pError);
}