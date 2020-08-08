#include "MongoDB.h"

using std::string;

MongoDB::MongoDB(mongoc_client_t *pClient, const string name) {
	_pClient = pClient;
	_name = name;
	_pDatabase = mongoc_client_get_database(pClient, name.c_str());
}

MongoDB::~MongoDB() {
	mongoc_database_destroy(_pDatabase);
}

string MongoDB::getName() const {
	return _name;
}

mongoc_database_t* MongoDB::getMongo() const {
	return _pDatabase;
}

MongoCollection* MongoDB::getCollection(const string name) {
	const auto found = _collections.find(name);

	//Already have this collection.
	if (found != _collections.end()) {
		return found->second.get();
	}

	//Create new collection
	mongoc_collection_t* p = mongoc_database_get_collection(_pDatabase, name.c_str());
	_collections[name] = std::make_unique<MongoCollection>(this, name, p);
	return _collections[name].get();
}

MongoCollection* MongoDB::createCollection(const string name, const bson_t *pOptions, bson_error_t *pError) {
	mongoc_collection_t *p = mongoc_database_create_collection(_pDatabase, name.c_str(), pOptions, pError);
	if (p) {
		_collections[name] = std::make_unique<MongoCollection>(this, name, p);
		return _collections[name].get();
	}
	return nullptr;
}

const bool MongoDB::hasCollection(const char *pName, bson_error_t *pError) const {
	return mongoc_database_has_collection(_pDatabase, pName, pError);
}

const bool MongoDB::getCollectionNames(std::vector<string> &namesOut, bson_error_t *pError) const {
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