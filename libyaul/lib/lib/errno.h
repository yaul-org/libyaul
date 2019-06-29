#ifndef _ERRNO_H_
#define _ERRNO_H_

#include <sys/cdefs.h>

#include <stdint.h>

typedef int32_t error_t;

__BEGIN_DECLS

#define EBUSY           (16)   /* Device or resource busy */
#define EINVAL          (22)   /* Invalid argument */
#define ENFILE          (23)   /* Too many open files in system */
#define ENOSPC          (28)   /* No space left on device */
#define EDOM            (33)   /* Mathematics argument out of domain of function */
#define ERANGE          (34)   /* Result too large */
#define ENOSYS          (88)   /* Function not implemented */
#define ENOBUFS         (105)  /* No buffer space available */
#define ECONNABORTED    (113)  /* Software caused connection abort */
#define EOVERFLOW       (139)  /* Value too large for defined data type */
#define ECANCELED       (140)  /* Operation canceled */
#define ENOTRECOVERABLE (141)  /* State not recoverable */
#define EWOULDBLOCK     EAGAIN /* Operation would block */

__END_DECLS

#endif /* !_ERRNO_H_ */
