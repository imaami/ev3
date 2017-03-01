#include "ev3_server.h"
#include "ev3_common.h"
#include "ev3_port.h"
#include "ev3_mem.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdlib.h> // aligned_alloc(), free()
#include <string.h> // memset(), strerror()
#include <stdint.h> // uint_fast8_t
#include <stddef.h> // size_t
#include <unistd.h> // sysconf()

struct ev3_server {
	ev3_port_t  port[8];
	uint8_t    *buf;
	size_t      buf_len;
};

static const char *EV3_NOERRNO = "Unknown error";
static size_t EV3_PAGESIZE = 0;

EV3_INLINE size_t
ev3_page_size (void);

ev3_server_t *
ev3_server_alloc (void)
{
	ev3_server_t *s = NULL;
	int e = 0;
	size_t buf_len = 0;
	uint8_t *buf = NULL;

	s = (ev3_server_t *)ev3_aligned_alloc (4, sizeof (ev3_server_t), &e);
	if (s == NULL) {
		ERR_ (-2, "Can't allocate memory for server object: %s",
		      (e != 0) ? strerror (e) : EV3_NOERRNO);
		return NULL;
	}

	for (buf_len = ev3_page_size(); buf_len < 8192; buf_len <<= 1);
	buf = (uint8_t *)ev3_aligned_alloc (EV3_PAGESIZE, buf_len, &e);

	if (buf == NULL) {
		ERR_ (-3, "Can't allocate memory for work buffer: %s",
		      (e != 0) ? strerror (e) : EV3_NOERRNO);
		free ((void *)s);
		s = NULL;
		e = 0;

	} else {
		memset (s, 0, sizeof (ev3_server_t));
		s->buf = buf;
		s->buf_len = buf_len;
		buf = NULL;
	}

	buf_len = 0;

	return s;
}

void
ev3_server_init (ev3_server_t *server)
{
	if (server != NULL) {
		uint_fast8_t i = 0;
		do {
			ev3_port_t *port = &server->port[i];
			ev3_port_init (port, i);
		} while (++i < 8);
	}
}

void
ev3_server_free (ev3_server_t **server)
{
	if (server != NULL) {
		ev3_server_t *s = *server;
		if (s != NULL) {
			if (s->buf != NULL) {
				if (s->buf_len > 0) {
					memset (s->buf, 0, s->buf_len);
				}
				free (s->buf);
			}
			memset (s, 0, sizeof (ev3_server_t));
			free (s);
			s = NULL;
			*server = s;
		}
	}
}

EV3_INLINE size_t
ev3_page_size (void)
{
	if (EV3_PAGESIZE == 0) {
		errno = 0;
		long page_size = sysconf (_SC_PAGESIZE);

		if (page_size > 0) {
			EV3_PAGESIZE = (size_t)page_size;

		} else {
			if (errno != 0) {
				ERR_ (-7, "sysconf(_SC_PAGESIZE): %s",
				      strerror (errno));
				errno = 0;
			}
			EV3_PAGESIZE = 4096;
		}
	}

	return EV3_PAGESIZE;
}

#ifdef __cplusplus
}
#endif // __cplusplus
