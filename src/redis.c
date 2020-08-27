#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <hiredis/hiredis.h>

#include "../types/string.h"

#include "../redis.h"

#pragma region utils

// creates a new c string with the desired format, as in printf
static char *redis_c_string_create (const char *format, ...) {

	char *fmt = NULL;

	if (format != NULL) fmt = strdup (format);
	else fmt = strdup ("");

	va_list argp;
	va_start (argp, format);
	char oneChar[1] = { 0 };
	int len = vsnprintf (oneChar, 1, fmt, argp);
	if (len < 1) return NULL;
	va_end (argp);

	char *str = (char *) calloc (len + 1, sizeof (char));
	if (!str) return NULL;

	va_start (argp, format);
	vsnprintf (str, len + 1, fmt, argp);
	va_end (argp);

	free (fmt);

	return str;

}

// prints a red error message to stderr
static void redis_log_error (const char *msg) {

	if (msg) fprintf (stderr, "\x1b[31m" "[REDIS][ERROR]: " "%s\n" "\x1b[0m", msg);

}

#pragma endregion

#pragma region status

static RedisStatus status = REDIS_STATUS_DISCONNECTED;

RedisStatus redis_get_status (void) { return status; }

#pragma endregion

#pragma region values

static redisContext *redis_context = NULL;

static String *hostname = NULL;

void redis_set_hostname (const char *host) {

	if (host) {
		str_delete (hostname);
		hostname = host ? str_new (host) : NULL;
	}

}

static int port = 0;

void redis_set_port (int p) { port = p; }

static struct timeval timeout = { 1, 500000 };

// sets the connection timeout
// default to 1.5 seconds
void redis_set_timeout (long seconds, long miliseconds) {

	timeout.tv_sec = seconds;
	timeout.tv_usec = miliseconds * 1000;

}

#pragma endregion

#pragma region connection

// connects to the redis server
// returns 0 on success, 1 on error
unsigned int redis_connect (void) {

	unsigned int retval = 1;

	if (status == REDIS_STATUS_DISCONNECTED) {
		redis_context = redisConnectWithTimeout (
			hostname->str, 
			port,
			timeout
		);

		if (redis_context) {
			if (redis_context->err) {
				char *s = redis_c_string_create ("Connection error: %s\n", redis_context->errstr);
				if (s) {
					redis_log_error (s);
					free (s);
				}

				redisFree (redis_context);
			}

			else {
				// success
				status = REDIS_STATUS_CONNECTED;
				retval = 0;
			}
		}

		else {
			redis_log_error ("Failed to connect to redis!");
		}
	}

	else {
		redis_log_error ("Already connected to redis!");
	}

	return retval;

}

// disconnects from the redis server
// returns 0 on success, 1 on error
unsigned int redis_disconnect (void) {

	unsigned int retval = 1;

	if (redis_context) {
		if (status == REDIS_STATUS_CONNECTED) {
			// disconnects and frees the context
			redisFree (redis_context);
			redis_context = NULL;

			retval = 0;
		}
	}

	return retval;

}

// pings the db to test for a success connection
// returns 0 on success, 1 on error
unsigned int redis_ping_db (void) {

	unsigned int retval = 1;

	if (redis_context) {
		redisReply *reply = (redisReply *) redisCommand (redis_context, "PING");
		if (reply) {
			fprintf (stdout, "\n[REDIS][PING]: %s\n", reply->str);
			freeReplyObject (reply);

			retval = 0;
		}
	}

	return retval;

}

#pragma endregion

#pragma region CRUD

// sets a new simple key value
// returns 0 on success, 1 on error
unsigned int redis_set_key (const char *key, const char *value) {

	unsigned int retval = 1;

	if (key && value) {
		redisReply *reply = (redisReply *) redisCommand (
			redis_context, 
			"SET %s %s", 
			key, value
		);

		if (reply) {
			printf ("SET: %s\n", reply->str);

			freeReplyObject (reply);

			retval = 0;
		}
		
	}

	return retval;

}

// gets a simple value for a key
// returns a newly allocated string that should be free
String *redis_get_value (const char *key) {

	String *retval = NULL;

	if (key) {
		redisReply *reply = (redisReply *) redisCommand (
			redis_context, 
			"SET %s", 
			key
		);

		if (reply) {
			printf ("GET: %s\n", reply->str);

			freeReplyObject (reply);

			retval = str_new (reply->str);
		}
	}

	return retval;

}

#pragma endregion