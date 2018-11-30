#include <stdlib.h>
#include <string.h>
#include <UnitTest++/UnitTest++.h>
#define  NO_HIPPOMOCKS_NAMESPACE
#include <HippoMocks/hippomocks.h>

#include "../headers/pixelfmt.h"
#include "../headers/tga_file.h"

using namespace OSn;

SUITE(PixelFmt)
{
	TEST (convert)
	{
		{
			Color32 out;
			out.value = PixelFmt::convert(RGBA(0x33221144), &tga_rgba32, &GFX::Formats::rgba32);

			CHECK (out.red   == 0x11);
			CHECK (out.green == 0x22);
			CHECK (out.blue  == 0x33);
			CHECK (out.alpha == 0x44);

			CHECK (out.value == RGBA(0x11223344));
		}

/*		{
			Color in;
		}
		CHECK (PixelFmt::convert(RGBA(0xff00ffff), &GFX::Formats::rgba32, &tga_rgba16) == 0x);
*/	}
}
