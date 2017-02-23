#ifndef _EV3_COMMON_H
#define _EV3_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdio.h>

#define ERR(fmt, args...) do{fprintf(stderr,"ERROR: %s:%d:%s: " fmt "\n",__FILE__,__LINE__,__func__ , ##args);}while(0)
#define ERR_(off, fmt, args...) do{fprintf(stderr,"ERROR: %s:%d:%s: " fmt "\n",__FILE__,__LINE__+(off),__func__ , ##args);}while(0)
#define MSG(fmt, args...) do{printf("%s: " fmt "\n",__func__ , ##args);}while(0)

#define EV3_INLINE __attribute__((always_inline)) static inline

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _EV3_COMMON_H
