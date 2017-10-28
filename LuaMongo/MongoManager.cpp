#include "MongoManager.h"

#define BUFFER_SIZE 128

/*************************************
*************** STATIC ***************
**************************************/

String MongoManager::getUri(const String host, const int port) {
	char uri[BUFFER_SIZE];
	sprintf_s(uri, BUFFER_SIZE, "mongodb://%s:%d", host.c_str(), port);
	return uri;
}

/*************************************
*************** PUBLIC ***************
**************************************/

MongoManager::MongoManager() {
	mongoc_init();
}

MongoManager::~MongoManager() {
	destroyClient();
	mongoc_cleanup();
}

MongoDB* MongoManager::getDatabase(const String name) {
	if (_pClient) {
		std::unordered_map<String, MongoDB*>::const_iterator found = _databases.find(name);
		//Already have this DB.
		if (found != _databases.end()) {
			return found->second;
		}
		//Create new DB
		MongoDB *pdB = new MongoDB(_pClient, name);
		_databases[name] = pdB;
		return pdB;
	}
	return nullptr;
}

const bool MongoManager::connect(const String host, const int port, bson_t *pReply, bson_error_t *pError) {
	return connect(getUri(host, port), pReply, pError);
}

const bool MongoManager::connect(const String uri, bson_t *pReply, bson_error_t *pError) {
	//Clean old states first
	destroyClient();
	_uri = uri;
	_pClient = mongoc_client_new(uri.c_str());
	mongoc_client_set_appname(_pClient, "LuaDB");
	//Return server status
	return mongoc_client_get_server_status(_pClient, nullptr, pReply, pError);
}

const bool MongoManager::getDatabaseNames(std::vector<String> &namesOut, bson_error_t *pError) {
	char **strv;
	if ((strv = mongoc_client_get_database_names(_pClient, pError))) {
		for (int i = 0; strv[i]; i++) {
			namesOut.push_back(strv[i]);
		}
	}
	bson_strfreev(strv);
	return !!strv;
}

/*************************************
*************** PRIVATE **************
**************************************/

void MongoManager::destroyClient() {
	if (_pClient) {
		for (auto it : _databases) {
			delete it.second;
		}
		mongoc_client_destroy(_pClient);
	}
}