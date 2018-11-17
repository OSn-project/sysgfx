#ifndef OSn_RECT_H_
#define OSn_RECT_H_

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
