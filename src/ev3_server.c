#include "ev3_server.h"
#include "ev3_common.h"
#include "ev3_port.h"
#include "ev3_mem.h"
#include "ev3_syspath.h"
#include "ev3_io.h"
#include "ev3_driver.h"

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

EV3_INLINE void
ev3_server_init (ev3_server_t *server,
                 uint8_t      *buf,
                 size_t        buf_len);

ev3_server_t *
ev3_server_new (void)
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
		ERR_ (-3, "Can't allocate memory for internal buffer: %s",
		      (e != 0) ? strerror (e) : EV3_NOERRNO);
		free ((void *)s);
		s = NULL;
		e = 0;

	} else {
		ev3_server_init (s, buf, buf_len);
		buf = NULL;
	}

	buf_len = 0;

	return s;
}

void
ev3_server_delete (ev3_server_t *server)
{
	if (server != NULL) {
		if (server->buf != NULL) {
			if (server->buf_len > 0) {
				memset (server->buf, 0, server->buf_len);
			}

			free (server->buf);
		}

		memset (server, 0, sizeof (ev3_server_t));

		free (server);
		server = NULL;
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

static EV3_SYSPATH_LIST(_ev3_server_syspaths) {
	EV3_SYSPATH("lego-sensor", "sensor"),
	EV3_SYSPATH("tacho-motor", "motor")
};

EV3_INLINE void
ev3_server_probe_syspath (ev3_server_t        *server,
                          const ev3_syspath_t *sp)
{
	uint8_t *buf = server->buf;
	size_t buf_len = server->buf_len;
	const char *subdir = sp->dir;
	size_t subdir_len = sp->dlen;
	const char *symlink_prefix = sp->lnk;
	size_t symlink_prefix_len = sp->llen;
	char path[64] = {'/','s','y','s','/','c','l','a','s','s','/','\0'};
	size_t off = sizeof ("/sys/class/") - 1;
	size_t have = sizeof (path) - sizeof ("/sys/class/"
	                                      "/"
	                                      "/address");
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

		strncpy (dest, "/address", sizeof ("/address"));

		if (ev3_realpath ((const char *)path, (char *)buf) != 0) {
		_next_dirent:
			continue;
		}

		n = strlen ((const char *)buf);
		if (n < sizeof ("address") - 1) {
			ERR_ (-7, "Can't get syspath: String returned by "
			          "realpath() too short for a valid path");
			continue;
		}
		n -= sizeof ("address") - 1; // offset for filename

		//MSG ("%s -> %s", path, (char *)buf);

		size_t alloc_len = ev3_port_syspath_buf_len_from_dir_len (n - 1);
		char *port_path_buf = aligned_alloc (4, alloc_len);
		if (!port_path_buf) {
			ERR ("aligned_alloc() failed");
			continue;
		}
		strncpy (port_path_buf, (const char *)buf, alloc_len);

		uint_fast8_t port_id;
		ev3_port_t *port;
		const ev3_driver_t *drv;

		(void)ev3_read_file (port_path_buf, buf, buf_len - 1);
		if (ev3_port_id_from_address ((const char *)buf, &port_id)) {
			strncpy (&port_path_buf[n], "driver_name",
			         sizeof ("driver_name"));
			if (ev3_read_file (port_path_buf, buf, buf_len - 1) &&
			    (drv = ev3_driver_by_name ((const char *)buf))) {
				port_path_buf[n] = '\0';
				port = &server->port[port_id];
				ev3_port_set_syspath_buf (port, port_path_buf,
				                          alloc_len, n);
				ev3_port_set_driver(port, drv);
				ev3_port_address (port, (char *)buf);
				MSG ("%s: %s (%s)", (char *)buf, drv->name,
				                    port->syspath.buf);
				continue;
			}
		}

		free (port_path_buf);

/*
		strncpy (&port_path_buf[n], "modes", sizeof ("modes"));
		if (read_file (port_path_buf, buf, buf_len - 1)) {
			MSG ("modes: %s", (char *)buf);
		}

		strncpy (&port_path_buf[n], "commands", sizeof ("commands"));
		if (read_file (port_path_buf, buf, buf_len - 1)) {
			MSG ("commands: %s", (char *)buf);
		}
*/
	}

_done:
	(void)ev3_closedir (&dir);
}

EV3_INLINE void
ev3_server_init (ev3_server_t *server,
                 uint8_t      *buf,
                 size_t        buf_len)
{
	uint_fast8_t i = 0;

	memset (server, 0, sizeof (ev3_server_t));

	do {
		ev3_port_t *port = &server->port[i];
		ev3_port_init (port, i);
	} while (++i < 8);

	server->buf = buf;
	server->buf_len = buf_len;

	for (size_t i = 0; i < sizeof (_ev3_server_syspaths) / sizeof (_ev3_server_syspaths[0]); ++i) {
		ev3_server_probe_syspath (server, &_ev3_server_syspaths[i]);
	}
}

#ifdef __cplusplus
}
#endif // __cplusplus
