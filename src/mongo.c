#include <stdlib.h>
#include <stdio.h>

#include <mongoc/mongoc.h>
#include <bson/bson.h>

#include "mongo.h"

#include "types/string.h"
#include "utils/utils.h"
#include "utils/log.h"

static MongoStatus status = MONGO_STATUS_DISCONNECTED;

MongoStatus mongo_get_status (void) { return status; }

static mongoc_uri_t *uri = NULL;
static mongoc_client_t *client = NULL;
static mongoc_database_t *database = NULL;

static String *host = NULL;

void mongo_set_host (const char *h) { if (h) host = str_new (h); }

static String *port = NULL;

void mongo_set_port (const char *p) { if (p) port = str_new (p); }

static String *username = NULL;

void mongo_set_username (const char *u) { if (u) username = str_new (u); }

static String *password = NULL;

void mongo_set_password (const char *pswd) { if (pswd) password = str_new (pswd); }

static String *db_name = NULL;

void mongo_set_db_name (const char *name) {

	if (name) db_name = str_new (name);

}

static String *app_name = NULL;

void mongo_set_app_name (const char *name) {

	if (name) app_name = str_new (name);

}

static String *uri_string = NULL;

void mongo_set_uri (const char *uri) {

	if (uri) uri_string = str_new (uri);

}

// generates a new uri string with the set values (username, password, host, port & db name)
// that can be used to set as the uri for a new connection
// returns the newly uri string (that should be freed) on success, NULL on error
char *mongo_uri_generate (void) {

	char *retval = NULL;

	if (host && port && db_name) {
		if (username && password) {
			retval = c_string_create ("mongodb://%s:%s@%s:%s/%s", 
				username->str, password->str, 
				host->str, port->str,
				db_name->str);
		}

		else {
			retval = c_string_create ("mongodb://%s:%s/%s", 
				host->str, port->str,
				db_name->str);
		}
	}

	return retval;

}

// pings the db to test for a success connection
// Possible connection problems -- failed to authenticate to the db
// returns 0 on success, 1 on error
int mongo_ping_db (void) {

	int retval = 1;

	if (client) {
		if (db_name) {
			bson_t *command, reply, *insert;
			bson_error_t error;

			command = BCON_NEW ("ping", BCON_INT32 (1));
			if (mongoc_client_command_simple (
				client, 
				db_name->str, 
				command, 
				NULL, 
				&reply, 
				&error)) {
				// success
				char *str = bson_as_json (&reply, NULL);
				if (str) {
					fprintf (stdout, "\n%s\n", str);
					free (str);
				}

				retval = 0;
			}

			else {
				char *status = c_string_create ("[MONGO] %s", error.message);
				if (status) {
					log_error (status);
					free (status);
				}
			}
		}

		else {
			log_error ("DB name hasn't been set! Use mongo_set_db_name ()");
		}
	}

	else {
		log_error ("Not connected to mongo! Call mongo_connect () first");
	}

	return retval;

}

// connect to the mongo db with db name
int mongo_connect (void) {

	int retval = 1;

	if (uri_string) {
		bson_error_t error;

		mongoc_init ();     // init mongo internals

		// safely create mongo uri object
		uri = mongoc_uri_new_with_error (uri_string->str, &error);
		if (!uri) {
			fprintf (stderr,
				"failed to parse URI: %s\n"
				"error message:       %s\n",
				uri_string->str,
				error.message);
			return 1;
		}

		// create a new client instance
		client = mongoc_client_new_from_uri (uri);
		if (!client) {
			fprintf (stderr, "Failed to create a new client instance!\n");
			return 1;
		}

		// register the app name -> for logging info
		mongoc_client_set_appname (client, app_name->str);

		status = MONGO_STATUS_CONNECTED;

		retval = 0;
	}

	else {
		log_error ("Not uri string! Call mongo_set_uri () before attemting a connection");
	}

	return retval;

}

// disconnects from the db
void mongo_disconnect (void) {

	mongoc_database_destroy (database);
	mongoc_uri_destroy (uri);
	mongoc_client_destroy (client);

	str_delete (host);
	str_delete (port);
	str_delete (username);
	str_delete (password);
	
	str_delete (app_name);
	str_delete (uri_string);
	str_delete (db_name);

	mongoc_cleanup ();

	status = MONGO_STATUS_DISCONNECTED;

}

// opens handle to a mongo collection in the db
mongoc_collection_t *mongo_collection_get (const char *coll_name) {

	return mongoc_client_get_collection (client, db_name->str, coll_name);

}

// drops a collection deleting all of its data
// retuns 0 on success, 1 on error
int mongo_collection_drop (mongoc_collection_t *collection) {

	int retval = 1;

	bson_error_t error;
	if (mongoc_collection_drop (collection, &error)) {
		retval = 0;
	}

	else {
		char *status = c_string_create ("Failed to drop collection - %s", error.message);
		if (status) {
			log_error (status);
			free (status);
		}
	}

	return retval;

}

#pragma region CRUD

// counts the docs in a collection by a matching query
int64_t mongo_count_docs (mongoc_collection_t *collection, bson_t *query) {

	int64_t retval = 0;

	if (collection && query) {
		bson_error_t error;
		retval = mongoc_collection_count_documents (collection, query, NULL, NULL, NULL, &error);
		if (retval < 0) {
			log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, c_string_create ("%s", error.message));
			retval = 0;
		}

		bson_destroy (query);
	}

	return retval;

}

// inserts a document into a collection
int mongo_insert_document (mongoc_collection_t *collection, bson_t *doc) {

	int retval = 0;
	bson_error_t error;

	if (!mongoc_collection_insert_one (collection, doc, NULL, NULL, &error)) {
		log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, c_string_create ("%s", error.message));
		retval = 1;
	}

	bson_destroy (doc);

	return retval;

}

// use a query to find one doc
const bson_t *mongo_find_one (mongoc_collection_t *collection, bson_t *query) {

	const bson_t *doc = NULL;

	if (collection && query) {
		mongoc_cursor_t *cursor = mongoc_collection_find_with_opts (collection, query, NULL, NULL);

		mongoc_cursor_next (cursor, &doc);

		bson_destroy (query);
		mongoc_cursor_destroy (cursor);
	}

	return doc;

}

// use a query to find all matching documents
// returns a cursor that can be used to traverse the matching documents
mongoc_cursor_t *mongo_find_all_cursor (mongoc_collection_t *collection, bson_t *query, uint64_t *n_docs) {

	mongoc_cursor_t *cursor = NULL;
	*n_docs = 0;

	if (collection && query) {
		uint64_t count = mongo_count_docs (collection, bson_copy (query));
		if (count > 0) {
			mongoc_cursor_t *cursor = mongoc_collection_find_with_opts (collection, query, NULL, NULL);

			*n_docs = count;
		}
	}

	return cursor;

}

// use a query to find all matching documents
// an empty query will return all the docs in a collection
bson_t **mongo_find_all (mongoc_collection_t *collection, bson_t *query, uint64_t *n_docs) {

	bson_t **retval = NULL;
	*n_docs = 0;

	if (collection && query) {
		uint64_t count = mongo_count_docs (collection, bson_copy (query));
		if (count > 0) {
			retval = (bson_t **) calloc (count, sizeof (bson_t *));
			for (uint64_t i = 0; i < count; i++) retval[i] = bson_new ();

			const bson_t *doc;
			mongoc_cursor_t *cursor = mongoc_collection_find_with_opts (collection, query, NULL, NULL);

			uint64_t i = 0;
			while (mongoc_cursor_next (cursor, &doc)) {
				// add the matching doc into our retval array
				bson_copy_to (doc, retval[i]);
				i++;
			}

			*n_docs = count;

			bson_destroy (query);
			mongoc_cursor_destroy (cursor);
		}
	}

	return retval;

}

// updates a doc by a matching query with the new values;
// destroys query and update bson_t
int mongo_update_one (mongoc_collection_t *collection, bson_t *query, bson_t *update) {

	int retval = 1;

	if (collection && query && update) {
		bson_error_t error;

		if (mongoc_collection_update_one (collection, query, update, NULL, NULL, &error)) 
			retval = 0;

		else
			log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, c_string_create ("Failed to update doc: %s", error.message));

		bson_destroy (query);
		bson_destroy (update);
	}

	return retval;

}

// deletes one matching document by a query
int mongo_delete_one (mongoc_collection_t *collection, bson_t *query) {

	int retval = 0;

	if (collection && query) {
		bson_error_t error;

		if (!mongoc_collection_delete_one (collection, query, NULL, NULL, &error)) {
			log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, c_string_create ("Delete failed: %s", error.message));
			retval = 1;
		}

		bson_destroy (query);
	}

	return retval;

}

// deletes all the query matching documents
// destroys the query
int mongo_delete_many (mongoc_collection_t *collection, bson_t *query) {

	int retval = 0;

	if (collection && query) {
		bson_error_t error;

		if (!mongoc_collection_delete_many (collection, query, NULL, NULL, &error)) {
			log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, c_string_create ("Delete failed: %s", error.message));
			retval = 1;
		}

		bson_destroy (query);
	}

	return retval;

}

#pragma endregion