#ifndef OSn_TGA_FILE_H_
#define OSn_TGA_FILE_H_

#include <osndef.h>
#include "../headers/pixelfmt.h"

#define TGA_INDEXED		0b0001
#define TGA_RGBA		0b0010
#define TGA_GRAYSCALE	0b0011
#define TGA_ISRLE		0b1000

struct TGAHeader
{
	uint8 id_len;
	uint8 cmap_type;
	uint8 img_type;

	struct __attribute__((packed)) {
		uint16 first_entry_idx;
		uint16 length;
		uint8  bpp;
	} cmap_info;

	uint16 x_orig, y_orig;

	uint16 width, height;

	uint8 bpp;
	uint8 image_descr;

	/*  */
	const OSn::GFX::PixelFmt *get_pixelfmt();	// Note, does not work for indexed images as the pallette varies on a per-image basis.
	inline bool is_indexed() { return (this->img_type & 0b00000011) == TGA_INDEXED; }
	inline size_t cmap_size() { return this->cmap_info.length * (this->cmap_info.bpp / 8); }
};

struct TGAMeta
{
	/* A struct which is passed	between this library and			*
	 * applications which may want to access TGA-specific metadata.	*/

	uint16 x_orig, y_orig;

	uint8 id_len;
	void *img_id;
};

extern const OSn::GFX::PixelFmt tga_rga16;
extern const OSn::GFX::PixelFmt tga_rgb24;
extern const OSn::GFX::PixelFmt tga_rgba32;

#endif

	//inline bool has_cmap() const { return cmap_type  }
