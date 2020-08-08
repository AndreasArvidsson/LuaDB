#pragma once
#include <string>
#include "MongoDB.h"

class MongoManager {
public:
	static std::string getUri(const std::string host = "localhost", int port = 27017);

	MongoManager();
	~MongoManager();

	const bool connect(const std::string uri, bson_t *pReply = nullptr, bson_error_t *pError = nullptr);
	const bool connect(const std::string host, int port, bson_t *pReply = nullptr, bson_error_t *pError = nullptr);
	MongoDB* getDatabase(const std::string name);
	const bool getDatabaseNames(std::vector<std::string> &namesOut, bson_error_t *pError = nullptr);

private:
	std::unordered_map<std::string, MongoDB*> _databases;
	std::string _uri;
	mongoc_client_t *_pClient;

	void destroyClient();
	
};
