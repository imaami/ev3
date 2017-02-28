#include "ev3_server.h"
#include "ev3_common.h"
#include "ev3_port.h"
#include "ev3_mem.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdlib.h> // aligned_alloc(), free()
#include <string.h> // memset()
#include <stdint.h> // uint_fast8_t

struct ev3_server {
	ev3_port_t port[8];
};

ev3_server_t *
ev3_server_alloc (void)
{
	ev3_server_t *s;
	int e;

	s = (ev3_server_t *)ev3_aligned_alloc (4, sizeof (ev3_server_t), &e);
	if (s == NULL) {
		const char *msg = (e != 0) ? strerror (e) : "Unknown error";
		ERR_ (-3, "Can't allocate memory for server object: %s", msg);
	}

	return s;
}

void
ev3_server_init (ev3_server_t *server)
{
	if (server != NULL) {
		memset (server, 0, sizeof (ev3_server_t));

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
		ev3_free (*server);
	}
}

#ifdef __cplusplus
}
#endif // __cplusplus
