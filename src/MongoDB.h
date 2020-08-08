#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include "MongoCollection.h"

class MongoDB {
public:
	MongoDB(mongoc_client_t *pClient, const std::string name);
	~MongoDB();

	std::string getName() const;
	mongoc_database_t* getMongo() const;

	MongoCollection* getCollection(const std::string name);
	MongoCollection* createCollection(const std::string name, const bson_t *pOptions, bson_error_t *pError = nullptr);
	const bool getCollectionNames(std::vector<std::string> &namesOut, bson_error_t *pError = nullptr) const;
	const bool hasCollection(const char *pName, bson_error_t *pError) const;
	const bool runCommand(const bson_t *pCommand, bson_t *pReply, bson_error_t *pError = nullptr) const;
	const bool drop(bson_error_t *pError = nullptr) const;

private:
	std::unordered_map<std::string, std::unique_ptr<MongoCollection>> _collections;
	std::string _name;
	mongoc_database_t *_pDatabase;
	mongoc_client_t *_pClient;
};