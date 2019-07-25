/* The OSn Color struct is used to store the RGBA	*
 * value of a single colour. It can be cast to 		*
 * uint32 if necessary. Deep colours (ie. with a 	*
 * bit-depth higher than 32) will need to be dealt	*
 * with specially.									*/

#ifndef OSn_COLOR_H_
#define OSn_COLOR_H_

#include <osndef.h>
#include <endian.h>

#define RGBA(C) htobe32(C)	// This allows the use of 32-bit integers as HTML-style RGBA literals. For example, dark yellow would be RGBA(0xffff0000)
#define RGB(C) RGBA(((C) << 8) & 0x000000ff)

namespace OSn
{
	namespace GFX
	{
		struct PixelFmt;
	}

	union Color32
	{
		struct {
			uint8 red;
			uint8 green;
			uint8 blue;
			uint8 alpha;	// 0x00 = opaque; 0xff = transparent
		};

		uint32 value;

		static const GFX::PixelFmt format;
	};

	union __attribute__((packed)) Color24
	{
		struct __attribute__((packed)) {
			uint8 red;
			uint8 green;
			uint8 blue;
		};

		uint32 value : 24;

		static const GFX::PixelFmt format;
	};
}

#endif
