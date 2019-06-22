#include <stdlib.h>
#include <string.h>
#include <UnitTest++/UnitTest++.h>
#define  NO_HIPPOMOCKS_NAMESPACE
#include <HippoMocks/hippomocks.h>

#include "../headers/bitmap.h"
#include "../headers/color.h"

using namespace OSn;

void randomize_bmp(Bitmap *bmp)
{
	for (uint8 *byte = bmp->bytes; byte - bmp->bytes < bmp->pitch * bmp->width; byte++)
	{
		*byte = (uint8) rand();
	}
}

SUITE(Bitmap)
{
	void *dummy_malloc(size_t)
	{
		return (void *) 1;
	}
	
	TEST (constructor)
	{
		MockRepository mocks;
		
		GFX::PixelFmt fmt = { .bpp = 11, .bypp = 2 };
		Bitmap bmp(15, 37, &fmt);
		
		mocks.ExpectCallFunc(malloc).With(347).Do(dummy_malloc);
		
		CHECK(bmp.width  == 15);
		CHECK(bmp.pitch  == 30);	// 15 * 2bypp
		CHECK(bmp.height == 37);
		CHECK(bmp.flags  == BMP_OWNDATA);
		CHECK(bmp.format == &fmt);
		CHECK(bmp.data   != NULL);
	}

	TEST(get_set_pixel)
	{
//		Bitmap bmp(100, 100, &Color32::format);
//		randomize_bmp(&bmp);
//
//		dword val;
//		bmp.get_pixel(19, 43, &val);
//		bmp.set_pixel(31, 55,  val);
//
//		CHECK(*((uint32) bmp.get_pixel(31, 55)) == *((uint32) bmp.get_pixel(19, 43)));;
	}
}
