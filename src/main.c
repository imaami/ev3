#include "ev3_server.h"
#include <stdlib.h>

int
main (int    argc,
      char **argv)
{
	ev3_server_t *ev3;
	ev3 = ev3_server_new ();
	ev3_server_delete (ev3);
	return 0;
}
