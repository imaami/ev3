#ifndef _EV3_SYSPATH_H
#define _EV3_SYSPATH_H

#include "ev3_common.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stddef.h> // size_t

typedef struct ev3_syspath {
	const char   dir[12];
	const size_t dlen;
	const char   lnk[8];
	const size_t llen;
} ev3_syspath_t;

#define EV3_SYSPATH_LIST(X) const ev3_syspath_t X[] =
#define EV3_SYSPATH(D, L) {.dir=D,.dlen=sizeof(D)-1,.lnk=L,.llen=sizeof(L)-1}

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _EV3_SYSPATH_H
