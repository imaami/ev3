#ifndef _EV3_IO_H
#define _EV3_IO_H

#include "ev3_common.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdio.h>   // FILE, fopen(), fileno(), fread(), ferror(), feof(), fclose()
#include <errno.h>   // errno
#include <limits.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <dirent.h>    // DIR, opendir(), readdir(), closedir()

#ifndef EV3_IO_DEBUG
# define EV3_IO_DEBUG
#endif // EV3_IO_DEBUG

#ifdef EV3_IO_DEBUG
# include <string.h>  // strerror()
#endif // EV3_IO_DEBUG

extern size_t
ev3_read_file (char    *path,
               uint8_t *buf,
               size_t   buf_len);

EV3_INLINE int
ev3_fopen (const char  *path,
           const char  *mode,
           FILE       **pfp)
{
	int e, r;
	FILE *fp;

	e = errno;
	errno = 0;

	if ((fp = fopen (path, mode)) == NULL) {
		r = errno;
#ifdef EV3_IO_DEBUG
		ERR_ (-3, "fopen(\"%s\", \"%s\"): %s", path, mode, strerror (r));
#endif // EV3_IO_DEBUG
	} else {
		r = 0;
		*pfp = fp;
	}

	errno = e;

	return r;
}

EV3_INLINE int
ev3_fread (FILE    *fp,
           uint8_t *buf,
           size_t   buf_len,
           size_t  *read_len)
{
	int e = errno, r = 0;
	size_t n = 0;

	do {
		if (buf_len > 0) {
			errno = 0;
			n = fread ((void *)buf, 1, buf_len, fp);
			r = errno;

			if (n != buf_len && ferror (fp)) {
				if (r == 0) {
					r = ENODATA;
#ifdef EV3_IO_DEBUG
					ERR_ (-7, "fread() failed");
				} else {
					ERR_ (-9, "fread(): %s", strerror (r));
#endif // EV3_IO_DEBUG
				}
				break;
			}

			if (feof (fp)) {
				r = 0;
				break;
			}
		}

		r = ENOBUFS;
	} while (false);

	*read_len = n;
	errno = e;

	return r;
}

EV3_INLINE int
ev3_fileno (FILE *fp,
            int  *pfd)
{
	int e, r;
	int fd;

	e = errno;
	errno = 0;

	if ((fd = fileno (fp)) < 0) {
		r = errno;
#ifdef EV3_IO_DEBUG
		ERR_ (-3, "fileno(): %s", strerror (r));
#endif // EV3_IO_DEBUG
	} else {
		r = 0;
		*pfd = fd;
	}

	errno = e;

	return r;
}

EV3_INLINE int
ev3_fclose (FILE **pfp)
{
	int e, r;

	e = errno;
	errno = 0;

	if (fclose (*pfp) != 0) {
		r = errno;
#ifdef EV3_IO_DEBUG
		ERR_ (-3, "fclose(): %s", strerror (r));
#endif // EV3_IO_DEBUG
	} else {
		r = 0;
		*pfp = NULL;
	}

	errno = e;

	return r;
}

EV3_INLINE int
ev3_opendir (const char  *path,
             DIR        **pdp)
{
	int e, r;
	DIR *dp;

	e = errno;
	errno = 0;

	if ((dp = opendir (path)) == NULL) {
		r = errno;
#ifdef EV3_IO_DEBUG
		ERR_ (-3, "opendir(\"%s\"): %s", path, strerror (r));
#endif // EV3_IO_DEBUG
	} else {
		r = 0;
		*pdp = dp;
	}

	errno = e;

	return r;
}

EV3_INLINE int
ev3_readdir (DIR            *dp,
             struct dirent **pep)
{
	int e, r;
	struct dirent *ep;

	e = errno;
	errno = 0;

	if ((ep = readdir (dp)) == NULL) {
		r = errno;
#ifdef EV3_IO_DEBUG
		if (r != 0) ERR_ (-3, "readdir(): %s", strerror (r));
#endif // EV3_IO_DEBUG
	} else {
		r = 0;
	}

	*pep = ep;
	errno = e;

	return r;
}

EV3_INLINE int
ev3_closedir (DIR **pdp)
{
	int e, r;

	e = errno;
	errno = 0;

	if (closedir (*pdp) != 0) {
		r = errno;
#ifdef EV3_IO_DEBUG
		ERR_ (-3, "closedir(): %s", strerror (r));
#endif // EV3_IO_DEBUG
	} else {
		r = 0;
		*pdp = NULL;
	}

	errno = e;

	return r;
}

EV3_INLINE int
ev3_realpath (const char *path,
              char       *dest)
{
	int e, r;

	e = errno;
	errno = 0;

	if (realpath (path, dest) == NULL) {
		r = errno;
#ifdef EV3_IO_DEBUG
		ERR_ (-3, "realpath(\"%s\"): %s", path, strerror (r));
#endif // EV3_IO_DEBUG
	} else {
		r = 0;
	}

	errno = e;

	return r;
}

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _EV3_IO_H
