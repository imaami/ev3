#ifndef _EV3_MEM_H
#define _EV3_MEM_H

#include "ev3_common.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdlib.h> // aligned_alloc()
#include <stddef.h> // size_t
#include <errno.h>  // errno

#ifndef EV3_MEM_DEBUG
# define EV3_MEM_DEBUG
#endif // EV3_MEM_DEBUG

#ifdef EV3_MEM_DEBUG
# include <string.h>  // strerror()
#endif // EV3_MEM_DEBUG

#define ev3_free(ptr)                         \
	do {                                  \
		if ((ptr) != NULL) {          \
			free ((void *)(ptr)); \
			(ptr) = NULL;         \
		}                             \
	} while (0)

EV3_INLINE void *
ev3_aligned_alloc (size_t  align,
                   size_t  size,
                   int    *err)
{
	int e, r;
	void *ptr;

	e = errno;
	errno = 0;

	if ((ptr = aligned_alloc (align, size)) == NULL) {
		r = errno;
#ifdef EV3_MEM_DEBUG
		ERR_ (-3, "aligned_alloc(%zu, %zu): %s", align, size, strerror (r));
#endif // EV3_MEM_DEBUG
	} else {
		r = 0;
	}

	*err = r;
	errno = e;

	return ptr;
}

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _EV3_MEM_H
