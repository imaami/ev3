#ifndef _EV3_PORT_H
#define _EV3_PORT_H

#include "ev3_common.h"
#include "ev3_driver.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdio.h>   // FILE
#include <stddef.h>  // size_t
#include <stdint.h>  // uint_fast8_t
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>  // strerror()

typedef struct ev3_port ev3_port_t;

struct ev3_port {
	uint_fast8_t id;
	struct {
		char   *buf;
		size_t  buf_len;
		size_t  dir_len;
	} syspath;
	const ev3_driver_t *drv;
};

EV3_INLINE void
ev3_port_init (ev3_port_t   *port,
               uint_fast8_t  id)
{
	port->id = id;

	port->syspath.buf = NULL;
	port->syspath.buf_len = 0;
	port->syspath.dir_len = 0;

	port->drv = NULL;
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

EV3_INLINE size_t
ev3_port_syspath_buf_len_from_dir_len (size_t len)
{
	size_t n = (len += 17) & 3;
	if (n != 0) {
		len += 4 - n;
	}
	return len;
}

EV3_INLINE void
ev3_port_set_syspath_buf (ev3_port_t *port,
                          char       *buf,
                          size_t      buf_len,
                          size_t      dir_len)
{
	port->syspath.buf = buf;
	port->syspath.buf_len = buf_len;
	port->syspath.dir_len = dir_len;
}

EV3_INLINE void
ev3_port_set_driver (ev3_port_t         *port,
                     const ev3_driver_t *drv)
{
	port->drv = drv;
}

EV3_INLINE char *
ev3_port_get_syspath_buf (ev3_port_t *port,
                          size_t      dir_len)
{
	char *buf = port->syspath.buf;
	size_t buf_len = ev3_port_syspath_buf_len_from_dir_len (dir_len);

	if (buf != NULL) {
		if (port->syspath.buf_len >= buf_len) {
			return buf;
		}

		free (buf);
		buf = NULL;
		ev3_port_set_syspath_buf (port, NULL, 0, 0);
	}

	errno = 0;

	if ((buf = aligned_alloc (4, buf_len)) == NULL) {
		ERR_ (-1, "Can't allocate port syspath"
		          " buffer: %s", strerror (errno));
		return NULL;
	}

	port->syspath.buf = buf;
	port->syspath.buf_len = buf_len;

	return buf;
}

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _EV3_PORT_H
