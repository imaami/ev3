#include "ev3_common.h"
#include "ev3_io.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stddef.h> // size_t
#include <stdint.h> // uint8_t

size_t
ev3_read_file (char    *path,
               uint8_t *buf,
               size_t   buf_len)
{
	size_t len = 0;
	FILE *fp = NULL;

	if (ev3_fopen ((const char *)path, "r", &fp) == 0) {
		(void)ev3_fread (fp, buf, buf_len, &len);
		(void)ev3_fclose (&fp);
	}

	buf[len] = 0;

	return len;
}

#ifdef __cplusplus
}
#endif // __cplusplus
