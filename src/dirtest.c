#include "ev3_common.h"
#include "ev3_io.h"

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>

static char *page_buf;
static size_t page_size;

static size_t
read_file (char   *path,
           char   *buf,
           size_t  buf_len)
{
	size_t len = 0;
	int err;
	FILE *fp;

	if ((err = ev3_fopen ((const char *)path, "r", &fp))) {
		ERR_ (-1, "Can't open %s: %s", path, strerror (err));

	} else {
		if ((err = ev3_fread (fp, (uint8_t *)buf, buf_len, &len))) {
			ERR_ (-2, "Can't read %s: %s", path, strerror (err));
		}

		if ((err = ev3_fclose (&fp))) {
			ERR_ (-1, "Can't close %s: %s", path, strerror (err));
		}
	}

	buf[len] = '\0';

	return len;
}

EV3_INLINE void
ev3_syspath_find_devices (const char *subdir,
                          size_t      subdir_len,
                          const char *symlink_prefix,
                          size_t      symlink_prefix_len)
{
	char path[64] = {'/','s','y','s','/','c','l','a','s','s','/','\0'};
	size_t off = sizeof ("/sys/class/") - 1;
	size_t have = sizeof (path) - sizeof ("/sys/class/"
	                                      "/"
	                                      "/driver_name");
	DIR *dir;
	int e;

	if (have < subdir_len) {
	_overflow:
		ERR ("Path buffer would overflow");
		return;
	}
	have -= subdir_len;
	if (symlink_prefix_len > 0) {
		if (have < symlink_prefix_len) {
			goto _overflow;
		}
		have -= symlink_prefix_len;
	}

	memcpy (&path[off], subdir, subdir_len);
	path[(off += subdir_len)] = '\0';

	if (ev3_opendir ((const char *)path, &dir) != 0) {
		return;
	}

	path[off++] = '/';

	if (symlink_prefix_len > 0) {
		memcpy (&path[off], symlink_prefix, symlink_prefix_len);
		off += symlink_prefix_len;
	}

	for (struct dirent *ent;;) {
		(void)ev3_readdir (dir, &ent);
		if (ent == NULL) {
			break;
		}

		if (ent->d_type != DT_LNK) {
			continue;
		}

		const char *name = (const char *)&ent->d_name[0];

		if (symlink_prefix_len > 0) {
			if (strncmp (name, symlink_prefix,
			             symlink_prefix_len) != 0) {
				continue;
			}
			name += symlink_prefix_len;
		}

		char *dest = &path[off];
		size_t n = have;

		while (*name) {
			if (n == 0) {
				ERR ("Path buffer would overflow");
				goto _next_dirent;
			}
			*dest++ = *name++;
			--n;
		}

		strncpy (dest, "/driver_name", sizeof ("/driver_name"));

		if (ev3_realpath ((const char *)path, page_buf) != 0) {
		_next_dirent:
			continue;
		}

		MSG ("%s -> %s", path, page_buf);

		size_t len = strlen (page_buf);
		size_t alloc_len = len + 5;
		if ((n = alloc_len & 3)) {
			alloc_len += 4 - n;
		}
		char *port_path_buf = aligned_alloc (4, alloc_len);
		if (!port_path_buf) {
			ERR ("aligned_alloc() failed");
			continue;
		}
		len -= (sizeof ("driver_name") - 1); // offset for filename
		strncpy (port_path_buf, page_buf, alloc_len);

		if (read_file (port_path_buf, page_buf, page_size - 1)) {
			MSG ("driver_name: %s", page_buf);
		}

		strncpy (&port_path_buf[len], "address", sizeof ("address"));
		if (read_file (port_path_buf, page_buf, page_size - 1)) {
			MSG ("address: %s", page_buf);
		}

		strncpy (&port_path_buf[len], "modes", sizeof ("modes"));
		if (read_file (port_path_buf, page_buf, page_size - 1)) {
			MSG ("modes: %s", page_buf);
		}

		strncpy (&port_path_buf[len], "commands", sizeof ("commands"));
		if (read_file (port_path_buf, page_buf, page_size - 1)) {
			MSG ("commands: %s", page_buf);
		}

		free (port_path_buf);
	}

_done:
	(void)ev3_closedir (&dir);
}

EV3_INLINE size_t
ev3_page_size (void)
{
	long s;

	errno = 0;
	if ((s = sysconf (_SC_PAGESIZE)) <= 0) {
		if (s < 0) ERR ("sysconf(_SC_PAGESIZE): %s", strerror (errno));
		s = 4096;
	}

	return (size_t)s;
}

EV3_INLINE bool
ev3_page_buf_alloc (char   **buf,
                    size_t  *len)
{
	size_t _len = ev3_page_size();
	char *_buf = aligned_alloc (_len, _len);

	if (!_buf) {
		ERR ("aligned_alloc() failed");
		return false;
	}

	*buf = _buf;
	*len = _len;

	return true;
}

int
main (int    argc,
      char **argv)
{
	if (!ev3_page_buf_alloc (&page_buf, &page_size)) {
		return EXIT_FAILURE;
	}

	const struct {
		const char *subdir;
		size_t      subdir_len;
		const char *symlink_prefix;
		size_t      symlink_prefix_len;
	} syspaths[2] = {
		{
			.subdir             = "lego-sensor",
			.subdir_len         = sizeof ("lego-sensor") - 1,
			.symlink_prefix     = "sensor",
			.symlink_prefix_len = sizeof ("sensor") - 1
		},
		{
			.subdir             = "tacho-motor",
			.subdir_len         = sizeof ("tacho-motor") - 1,
			.symlink_prefix     = "motor",
			.symlink_prefix_len = sizeof ("motor") - 1
		}
	};

	for (size_t i = 0; i < sizeof (syspaths) / sizeof (syspaths[0]); ++i) {
		ev3_syspath_find_devices (syspaths[i].subdir,
		                          syspaths[i].subdir_len,
		                          syspaths[i].symlink_prefix,
		                          syspaths[i].symlink_prefix_len);
	}

	free (page_buf);

	return EXIT_SUCCESS;
}
