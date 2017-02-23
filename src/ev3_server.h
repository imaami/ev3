#ifndef _EV3_SERVER_H
#define _EV3_SERVER_H

#include "ev3_common.h"
#include "ev3_port.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <string.h> // memset()
#include <stdint.h> // uint_fast8_t
#include <stdio.h>  // printf()

typedef struct ev3_server ev3_server_t;

struct ev3_server {
	ev3_port_t port[8];
};

EV3_INLINE void
ev3_server_init (ev3_server_t *server)
{
	memset (server, 0, sizeof (ev3_server_t));

	uint_fast8_t i = 0;
	do {
		char addr[8];
		ev3_port_t *port = &server->port[i];
		ev3_port_init (port, i);
		ev3_port_address (port, addr);
		printf ("Initialized %s\n", addr);
	} while (++i < 8);
}

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _EV3_SERVER_H
