/* Barebones compatibility structure that OSn::Bitmap translates	*
 * itself to in order to use SDL_gfx functions.						*/

#ifndef SRC_SDLGFX_SDL_COMPAT_HH_
#define SRC_SDLGFX_SDL_COMPAT_HH_

#include <osndef.h>

typedef uint32 Uint32;
typedef int32  Sint32;
typedef uint16 Uint16;
typedef int16  Sint16;
typedef uint8  Uint8;
typedef int8   Sint8;

struct SDL_BlitMap;

struct SDL_Rect
{
	int32 x,y,w,h;
};

struct SDL_Color
{
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 a;
};

struct SDL_Palette
{
    int ncolors;
    SDL_Color *colors;
    Uint32 version;
    int refcount;
};

struct SDL_PixelFormat
{
    Uint32 format;
    SDL_Palette *palette;
    Uint8 BitsPerPixel;
    Uint8 BytesPerPixel;
    Uint32 Rmask;
    Uint32 Gmask;
    Uint32 Bmask;
    Uint32 Amask;
    Uint8 Rloss;
    Uint8 Gloss;
    Uint8 Bloss;
    Uint8 Aloss;
    Uint8 Rshift;
    Uint8 Gshift;
    Uint8 Bshift;
    Uint8 Ashift;
};

struct SDL_Surface
{
    Uint32 flags;               /**< Read-only */
    SDL_PixelFormat _format;	// All allocated at once;
    SDL_PixelFormat *format;    // Points to the above.			/**< Read-only */
    int w, h;                   /**< Read-only */
    int pitch;                  /**< Read-only */
    void *pixels;               /**< Read-write */

    /** Application data associated with the surface */
    void *userdata;             /**< Read-write */

    /** information needed for surfaces requiring locks */
    int locked;                 /**< Read-only */
    void *lock_data;            /**< Read-only */

    /** clipping information */
    SDL_Rect clip_rect;         /**< Read-only */

    /** info for fast blit mapping to other surfaces */
    struct SDL_BlitMap *map;    /**< Private */

    /** Reference count -- used when freeing surface */
    int refcount;               /**< Read-mostly */
};

inline Uint32
SDL_MapRGBA(const SDL_PixelFormat * format, Uint8 r, Uint8 g, Uint8 b,
            Uint8 a)
{
    if (format->palette == NULL) {
        return (r >> format->Rloss) << format->Rshift
            | (g >> format->Gloss) << format->Gshift
            | (b >> format->Bloss) << format->Bshift
            | ((a >> format->Aloss) << format->Ashift & format->Amask);
    } else {
//        return SDL_FindColor(format->palette, r, g, b, a);
    	return 0;
    }
}
#define SDL_MapRGB(f,r,g,b) SDL_MapRGBA(f,r,g,b,0xff)

#define SDL_MUSTLOCK(s) false
#define SDL_LockSurface(s) 0
#define SDL_UnlockSurface(s) 0

#define SDL_BIG_ENDIAN 1
#define SDL_BYTEORDER  0

/* Inline conversion functions */

#include "../../headers/bitmap.h"
#include "../../headers/pixelfmt.h"

#include <endian.h>

namespace compat
{
	inline void to_sdlfmt(OSn::GFX::PixelFmt *i, SDL_PixelFormat *o)
	{
		o->format = 0;

		o->palette = NULL;	// TODO

		o->BitsPerPixel = i->bpp;
		o->BytesPerPixel = i->bypp;

		o->Rmask  = be32toh((uint32) *(i->rgba.r_mask));	// SDL stores
		o->Gmask  = be32toh((uint32) *(i->rgba.g_mask));
		o->Bmask  = be32toh((uint32) *(i->rgba.b_mask));
		o->Amask  = be32toh((uint32) *(i->rgba.a_mask));
		o->Rshift = i->rgba.r_shift;
		o->Gshift = i->rgba.g_shift;
		o->Bshift = i->rgba.b_shift;
		o->Ashift = i->rgba.a_shift;
		o->Rloss  = 8 - i->rgba.r_size;
		o->Gloss  = 8 - i->rgba.g_size;
		o->Bloss  = 8 - i->rgba.b_size;
		o->Aloss  = 8 - i->rgba.a_size;
	}

	inline SDL_Surface *to_sdl(OSn::Bitmap *bmp, SDL_Surface *s)	// inline *with the hope that it'll be optimized out*
	{
		/* Copies (translates) the format/size info from the OSn::Bitmap	*
		 * into the SDL_Surface. Surface shares the Bitmap's image data.	*
		 * The Surface is only to be used temporarily to interface with		*
		 * SDL_gfx functions->												*/

		s->w = bmp->width;
		s->h = bmp->height;
		s->pitch = bmp->pitch;

		s->pixels = bmp->data;

		/* Convert format */
		to_sdlfmt(bmp->format, &s->_format);
		s->format = &s->_format;

		/* Stuff that Bitmap doesn't support */
		s->locked = 0; s->lock_data = NULL;
		s->refcount = 1;

		s->clip_rect.x = 0;		// Bitmap doesn't have a clip rect
		s->clip_rect.y = 0;
		s->clip_rect.w = bmp->width;
		s->clip_rect.h = bmp->height;

		return s;
	}
//	OSn::Bitmap *from_sdl(SDL_Surface *sfc, OSn::Bitmap *bitmap);
}

#endif /* SRC_SDLGFX_SDL_COMPAT_HH_ */
