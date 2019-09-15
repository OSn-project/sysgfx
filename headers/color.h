/* The OSn Color struct is used to store the RGBA	*
 * value of a single colour. It can be cast to 		*
 * uint32 if necessary. Deep colours (ie. with a 	*
 * bit-depth higher than 32) will need to be dealt	*
 * with specially.									*/

#ifndef OSn_COLOR_H_
#define OSn_COLOR_H_

#include <osndef.h>
#include <endian.h>

#include <base/macros.h>

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

	inline Color32 RGBA(uint32 hexcode);
	inline Color32 RGBA(uint8 r, uint8 g, uint8 b, uint8 a = 255);
	inline Color32 RGBAf(float r, float g, float b, float a = 1);

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

	/* Inline functions */

	inline Color32 RGBA(uint32 hexcode)
	{
		Color32 col;

		col.red   = (hexcode & 0xff000000) >> 24;
		col.green = (hexcode & 0x00ff0000) >> 16;
		col.blue  = (hexcode & 0x0000ff00) >>  8;
		col.alpha = (hexcode & 0x000000ff) >>  0;

		return col;
	}

	inline Color32 RGBA(uint8 r, uint8 g, uint8 b, uint8 a)
	{
		Color32 col;

		col.red   = r;
		col.green = g;
		col.blue  = b;
		col.alpha = a;

		return col;
	}

	inline Color32 RGBAf(float r, float g, float b, float a)
	{
		Color32 col;

		col.red   = b_min(b_max(0, r), 1) * 255;
		col.green = b_min(b_max(0, g), 1) * 255;
		col.blue  = b_min(b_max(0, b), 1) * 255;
		col.alpha = b_min(b_max(0, a), 1) * 255;

		return col;
	}
}

#endif
