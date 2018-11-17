#include <stdlib.h>
#include <string.h>
#include <UnitTest++/UnitTest++.h>
#define  NO_HIPPOMOCKS_NAMESPACE
#include <HippoMocks/hippomocks.h>

#include "../.junk/bitmap.h"

SUITE(OSn_Bitmap)
{
	void *dummy_malloc(size_t)
	{
		return (void *) 1;
	}
	
	TEST (bmp_init)
	{
		MockRepository mocks;
		
		OSn_PixelInfo info = { .bpp = 5 };
		OSn_Bitmap bmp;
		
		mocks.ExpectCallFunc(malloc).With(347).Do(dummy_malloc);
		
		bmp_init(&bmp, &info, 15, 37, NULL);
		
		CHECK(bmp.width  == 15);
		CHECK(bmp.height == 37);
		CHECK(bmp.flags  == 0b00000001);
		CHECK(bmp.fmt_info == &info);
		CHECK(bmp.data   != NULL);
	}
}
