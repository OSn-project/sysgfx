#ifndef __OSN_BITMAP_RECT_H__
#define __OSN_BITMAP_RECT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <osndef.h>

typedef struct
{
	int32 x1;
	int32 y1;
	int32 x2;
	int32 y2;
} OSn_Rect;

#ifdef __cplusplus
}

namespace OSn
{
	typedef OSn_Rect Rect;
}
#endif
#endif
