#ifndef _REDIS_H_
#define _REDIS_H_

#include "types/string.h"

#pragma region status

typedef enum RedisStatus {

	REDIS_STATUS_DISCONNECTED		= 0,
	REDIS_STATUS_CONNECTED			= 1

} RedisStatus;

extern RedisStatus redis_get_status (void);

#pragma endregion

#pragma region values

extern void redis_set_hostname (const char *host);

extern void redis_set_port (int p);

// sets the connection timeout
// default to 1.5 seconds
extern void redis_set_timeout (long seconds, long miliseconds);

#pragma endregion

#pragma region connection

// connects to the redis server
// returns 0 on success, 1 on error
extern unsigned int redis_connect (void);

// disconnects from the redis server
// returns 0 on success, 1 on error
extern unsigned int redis_disconnect (void);

// pings the db to test for a success connection
// returns 0 on success, 1 on error
extern unsigned int redis_ping_db (void);

#pragma endregion

#pragma region CRUD

// sets a new simple key value
// returns 0 on success, 1 on error
extern unsigned int redis_set_key (const char *key, const char *value);

// gets a simple value for a key
// returns a newly allocated string that should be free
extern String *redis_get_value (const char *key);

#pragma endregion

#endif