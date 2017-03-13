#ifndef _EV3_DRIVER_H
#define _EV3_DRIVER_H

#include "ev3_common.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdio.h> // FILE

typedef struct ev3_driver {
	struct {
		union {
			FILE *mode;
			FILE *command;
		};
		union {
			FILE *modes;
			FILE *commands;
		};
	} fp;
	const char name[];
} ev3_driver_t;

#define EV3_INPUT_DRIVER(X, Y) const ev3_driver_t X = {\
	.fp.mode = NULL,\
	.fp.modes = NULL,\
	.name = Y\
}

#define EV3_OUTPUT_DRIVER(X, Y) const ev3_driver_t X = {\
	.fp.command = NULL,\
	.fp.commands = NULL,\
	.name = Y\
}

extern const ev3_driver_t ev3_driver_color;
extern const ev3_driver_t ev3_driver_ir;
extern const ev3_driver_t ev3_driver_l_motor;
extern const ev3_driver_t ev3_driver_m_motor;
extern const ev3_driver_t ev3_driver_touch;

EV3_INLINE const ev3_driver_t *
ev3_driver_by_name (const char *n)
{
	if (n[0] == 'l' && n[1] == 'e' && n[2] == 'g' && n[3] == 'o' &&
	    n[4] == '-' && n[5] == 'e' && n[6] == 'v' && n[7] == '3' &&
	    n[8] == '-') {
		switch (n[9]) {
		case 'c':
			if (n[10] == 'o' && n[11] == 'l' && n[12] == 'o' &&
			    n[13] == 'r') {
				return &ev3_driver_color;
			}
			break;

		case 'i':
			if (n[10] == 'r') {
				return &ev3_driver_ir;
			}
			break;

		case 'l':
			if (n[10] == '-' && n[11] == 'm' && n[12] == 'o' &&
			    n[13] == 't' && n[14] == 'o' && n[15] == 'r') {
				return &ev3_driver_l_motor;
			}
			break;

		case 'm':
			if (n[10] == '-' && n[11] == 'm' && n[12] == 'o' &&
			    n[13] == 't' && n[14] == 'o' && n[15] == 'r') {
				return &ev3_driver_m_motor;
			}
			break;

		case 't':
			if (n[10] == 'o' && n[11] == 'u' && n[12] == 'c' &&
			    n[13] == 'h') {
				return &ev3_driver_touch;
			}
		default:
			break;
		}
	}

	return NULL;
}

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _EV3_DRIVER_H
