#ifndef _EV3_PORT_H
#define _EV3_PORT_H

#include "ev3_common.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdio.h>   // FILE
#include <stddef.h>  // size_t
#include <stdint.h>  // uint_fast8_t
#include <stdbool.h>
#include <stdlib.h>

typedef struct ev3_port ev3_port_t;

struct ev3_port {
	uint_fast8_t id;
	struct {
		char   *buf;
		size_t  len;
	} syspath;
	struct {
		FILE *mode;
	} fp;
};

EV3_INLINE void
ev3_port_init (ev3_port_t   *port,
               uint_fast8_t  id)
{
	port->id = id;
}

EV3_INLINE bool
ev3_port_is_input (ev3_port_t *port)
{
	return (port->id & 4) == 0;
}

EV3_INLINE bool
ev3_port_is_output (ev3_port_t *port)
{
	return (port->id & 4) != 0;
}

EV3_INLINE char
ev3_port_id_char (ev3_port_t *port)
{
	char c = (char)port->id;
	return 49 + ((c & 4) << 2) + (c & 3);
}

EV3_INLINE bool
ev3_port_id_from_address (const char   *addr,
                          uint_fast8_t *id)
{
	switch (addr[0]) {
	case 'i':
		if (addr[1] == 'n' && addr[2] >= '1' && addr[2] <= '4') {
			*id = (uint_fast8_t)addr[2] - 0x31;
			return true;
		}
		return false;

	case 'o':
		if (addr[1] == 'u' && addr[2] == 't' && addr[3] >= 'A'
		    && addr[3] <= 'D') {
			*id = (uint_fast8_t)addr[3] - 0x3d;
			return true;
		}

	default:
		return false;
	}
}

EV3_INLINE void
ev3_port_address (ev3_port_t *port,
                  char       *addr)
{
	char c = (char)port->id;
	char o = c & 4;

	if (o != 0) {
		*addr++ = 'o';
		*addr++ = 'u';
		*addr++ = 't';
		o <<= 2;

	} else {
		*addr++ = 'i';
		*addr++ = 'n';
	}

	*addr++ = 49 + o + (c & 3);
	*addr = '\0';
}

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _EV3_PORT_H
