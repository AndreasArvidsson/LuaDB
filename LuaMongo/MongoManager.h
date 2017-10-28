#pragma once
#include "MongoDB.h"
#include "mongoc.h"

#pragma comment(lib,"mongoc.lib")
#pragma comment(lib,"bson.lib")

class MongoManager {
public:
	static String getUri(const String host = "localhost", int port = 27017);

	MongoManager();
	~MongoManager();

	const bool connect(const String uri, bson_t *pReply = nullptr, bson_error_t *pError = nullptr);
	const bool connect(const String host, int port, bson_t *pReply = nullptr, bson_error_t *pError = nullptr);
	MongoDB* getDatabase(const String name);
	const bool getDatabaseNames(std::vector<String> &namesOut, bson_error_t *pError = nullptr);

private:
	std::unordered_map<String, MongoDB*> _databases;
	String _uri;
	mongoc_client_t *_pClient;

	void destroyClient();
	
};
