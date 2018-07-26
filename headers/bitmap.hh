
namespace OSn
{
	struct BmpInfo
	{
		/* Used to represent the pixel format	*
		 * of a bitmap.							*/

		uint8 bpp;			// The number of bits per bixel

		/* Red chanel */
		uint32 r_mask;		// A mask representing the red component of the pixel
		uint8  r_shift;		// The number of bits to the right of the component in each pixel value
		uint8  r_loss;		// The number of bits lost from each component when packing the 8-bit color component in a pixel

		/* Green chanel */
		uint32 g_mask;
		uint8  g_shift;
		uint8  g_loss;

		/* Blue chanel */
		uint32 b_mask;
		uint8  b_shift;
		uint8  b_loss;

		/* Alpha chanel */
		uint32 a_mask;
		uint8  a_shift;
		uint8  a_loss;
	}
	
	class Bitmap
	{
		uint32 width;
		uint32 height;
		uint32 pitch;

	public:
		const BmpInfo *format_info;
		
		void *data;
		
	public:
		Bitmap(const BmpInfo *info, uint32_t width, uint32_t height);
		Bitmap(const Bitmap *bmp);
		Bitmap(const Bitmap *bmp, const Rect *area);
		~Bitmap();
		
		void get_pixel(uint32 x, uint32 y, void *out);
	};
	
	namespace GFX
	{
		void blit(const Bitmap *src, const Rect *src_rect, Bitmap *dest, const Rect *dest_rect);
		void hw_blit(const Bitmap *src, const Rect *src_rect, Bitmap *dest, const Rect *dest_rect);
		
		/* Drawing */
		void fill_rect(Bitmap *bmp, const Rect *rect, uint32 color);
		void draw_rect(Bitmap *bmp, const Rect *rect, uint32 color, uint32 width = 1);
		
		void line(Bitmap *bmp, uint32 x1, uint32 y1, uint32 x2, uint32 y2, uint32 color, uint32 width = 1);
		void aa_line(Bitmap *bmp, uint32 x1, uint32 y1, uint32 x2, uint32 y2, uint32 color, uint32 width = 1);
	}
}
