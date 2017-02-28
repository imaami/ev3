#include "ev3_server.h"
#include <stdlib.h>

int
main (int    argc,
      char **argv)
{
	ev3_server_t *ev3;
	if ((ev3 = ev3_server_alloc ()) != NULL) {
		ev3_server_init (ev3);
		ev3_server_free (&ev3);
	}
	return 0;
}
