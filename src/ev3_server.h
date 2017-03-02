#ifndef _EV3_SERVER_H
#define _EV3_SERVER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct ev3_server ev3_server_t;

extern ev3_server_t *
ev3_server_new (void);

extern void
ev3_server_delete (ev3_server_t *server);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _EV3_SERVER_H
