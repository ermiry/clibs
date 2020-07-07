#ifndef _MONGO_H_
#define _MONGO_H_

#include <mongoc/mongoc.h>
#include <bson/bson.h>

#include "collections/dllist.h"

typedef enum MongoStatus {

	MONGO_STATUS_DISCONNECTED		= 0,
	MONGO_STATUS_CONNECTED			= 1

} MongoStatus;

extern MongoStatus mongo_get_status (void);

extern void mongo_set_host (const char *h);

extern void mongo_set_port (const char *p);

extern void mongo_set_username (const char *u);

extern void mongo_set_password (const char *pswd);

extern void mongo_set_db_name (const char *name);

extern void mongo_set_app_name (const char *name);

extern void mongo_set_uri (const char *uri);

// generates a new uri string with the set values (username, password, host, port & db name)
// that can be used to set as the uri for a new connection
// returns the newly uri string (that should be freed) on success, NULL on error
extern char *mongo_uri_generate (void);

// pings the db to test for a success connection
// Possible connection problems -- failed to authenticate to the db
// returns 0 on success, 1 on error
extern int mongo_ping_db (void);

// connect to the mongo db with db name
extern int mongo_connect (void);

// disconnects from the db
extern void mongo_disconnect (void);

// opens handle to a mongo collection in the db
extern mongoc_collection_t *mongo_collection_get (const char *coll_name);

// drops a collection deleting all of its data
// retuns 0 on success, 1 on error
extern int mongo_collection_drop (mongoc_collection_t *collection);

#pragma region CRUD

// counts the docs in a collection by a matching query
extern int64_t mongo_count_docs (mongoc_collection_t *collection, bson_t *query);

// use a query to find all matching documents
// select is a dlist of strings used for document projection, _id is true by default and should not be incldued
// returns a cursor (should be destroyed) that can be used to traverse the matching documents
// query gets destroyed, select list remains the same
extern mongoc_cursor_t *mongo_find_all_cursor (mongoc_collection_t *collection, 
	bson_t *query, DoubleList *select,
	uint64_t *n_docs);

// use a query to find all matching documents
// an empty query will return all the docs in a collection
extern const bson_t **mongo_find_all (mongoc_collection_t *collection, 
	bson_t *query, DoubleList *select,
	uint64_t *n_docs);

// correctly destroys an array of docs got from mongo_find_all ()
extern void mongo_find_all_destroy_docs (bson_t **docs, uint64_t count);

// use a query to find one doc
// select is a dlist of strings used for document projection, _id is true by default and should not be incldued
// query gets destroyed, select list remains the same
const bson_t *mongo_find_one (mongoc_collection_t *collection, bson_t *query, DoubleList *select);

// inserts a document into a collection
// destroys document
// returns 0 on success, 1 on error
extern int mongo_insert_one (mongoc_collection_t *collection, bson_t *doc);

// inserts many documents into a collection
// returns 0 on success, 1 on error
extern int mongo_insert_many (mongoc_collection_t *collection, const bson_t **docs, size_t n_docs);

// updates a doc by a matching query with the new values
// destroys query and update documents
// returns 0 on success, 1 on error
extern int mongo_update_one (mongoc_collection_t *collection, bson_t *query, bson_t *update);

// updates all the query matching documents
// destroys the query and the update documents
// returns 0 on success, 1 on error
extern int mongo_update_many (mongoc_collection_t *collection, bson_t *query, bson_t *update);

// deletes one matching document by a query
// destroys the query document
// returns 0 on success, 1 on error
extern int mongo_delete_one (mongoc_collection_t *collection, bson_t *query);

// deletes all the query matching documents
// destroys the query
// returns 0 on success, 1 on error
extern int mongo_delete_many (mongoc_collection_t *collection, bson_t *query);

#pragma endregion

#endif