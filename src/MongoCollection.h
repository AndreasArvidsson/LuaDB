#pragma once
#include <string>
#include "mongoc.h"

class MongoDB;
class MongoWriteResult;

class MongoCollection {
public:
	MongoCollection(MongoDB *pMongoDB, const std::string& name, mongoc_collection_t *pCollection);
	~MongoCollection();

	std::string getName() const;
	std::string getFullName() const;
	MongoDB* getDatabase() const;

	mongoc_cursor_t* find(const bson_t *pQuery, const bson_t *pProjection = nullptr) const;
	mongoc_cursor_t* aggregate(const bson_t *pPipeline, const bson_t *pOptions = nullptr) const;
	const int64_t count(const bson_t *pQuery, bson_error_t *pError) const;
	const bool distinct(const std::string& key, const bson_t *pQuery, bson_t *pReply, bson_error_t *pError = nullptr) const;
	const bool insert(const bson_t *pDoc, bson_t *pWR = nullptr, bson_error_t *pError = nullptr) const;
	const bool save(const bson_t *pDoc, bson_t *pWR = nullptr, bson_error_t *pError = nullptr) const;
	const bool update(const bson_t *pQuery, const bson_t *pUpdate, const bson_t *pOptions= nullptr, bson_t *pWR = nullptr, bson_error_t *pError = nullptr) const;
	const bool remove(const bson_t *pQuery, bool justOne = false, bson_t *pWR = nullptr, bson_error_t *pError = nullptr) const;
	const bool renameCollection(const char *newName, bool dropExisting = false, bson_error_t *pError = nullptr) const;
	const bool runCommand(const bson_t *pCommand, bson_t *pReply = nullptr, bson_error_t *pError = nullptr) const;
	mongoc_bulk_operation_t* createBulkOperation(bool ordered = true) const;
	const bool executeBulkOperation(mongoc_bulk_operation_t *pBulk, bson_t *pReply = nullptr, bson_error_t *pError = nullptr) const;
	const bool drop(bson_error_t *pError = nullptr) const;
	const bool createIndex(bson_t *pKeys, bson_t *pOptions = nullptr, bson_t *pReply = nullptr, bson_error_t *pError = nullptr) const;
	const bool dropIndex(const char *pName, bson_error_t *pError = nullptr) const;

private:
	MongoDB *_pMongoDB;
	std::string _name;
	mongoc_collection_t *_pCollection;
};

