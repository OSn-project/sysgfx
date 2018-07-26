/* The OSn Color struct is used to store the RGBA	*
 * value of a single colour. It can be cast to 		*
 * uint32 if necessary. Deep colours (ie. with a 	*
 * bit-depth higher than 32) will need to be dealt	*
 * with specially.									*/

#ifndef __OSn_COLOR_H__
#define __OSn_COLOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <osndef.h>

typedef struct
{
	uint8 red;
	uint8 green;
	uint8 blue;
	uint8 alpha;	// 0x00 = transparent; 0xff = opaque
} OSn_Color32;

#ifdef __cplusplus
}

namespace OSn
{
	typedef OSn_Color32 Color32;
}
#endif

#endif
