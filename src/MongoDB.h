#pragma once
#include <unordered_map>
#include "MongoCollection.h"

class MongoDB {
public:
	MongoDB(mongoc_client_t *pClient, const String name);
	~MongoDB();

	String getName() const;
	mongoc_database_t* getMongo() const;

	MongoCollection* getCollection(const String name);
	MongoCollection* createCollection(const String name, const bson_t *pOptions, bson_error_t *pError = nullptr);
	const bool getCollectionNames(std::vector<String> &namesOut, bson_error_t *pError = nullptr) const;
	const bool hasCollection(const char *pName, bson_error_t *pError) const;
	const bool runCommand(const bson_t *pCommand, bson_t *pReply, bson_error_t *pError = nullptr) const;
	const bool drop(bson_error_t *pError = nullptr) const;

private:
	std::unordered_map<String, MongoCollection*> _collections;
	String _name;
	mongoc_database_t *_pDatabase;
	mongoc_client_t *_pClient;
};