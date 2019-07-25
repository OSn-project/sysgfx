#ifndef SYSGFX_ERRORS_H_
#define SYSGFX_ERRORS_H_

#include <osndef.h>

typedef int32 error_t;

#define E_OK 0
#define E_ERROR 1
#define E_NULLPTR 2

#define SYSGFX_ERRORS_BASE 3

#define E_BADFMT (SYSGFX_ERRORS_BASE + 0)	// Error, the image is of a bad pixel format.
#define E_ISRGBA (SYSGFX_ERRORS_BASE + 1)	// Error, the image is RGBA.
#define E_ISIDX  (SYSGFX_ERRORS_BASE + 2)	// Error, the image is indexed.

#endif
