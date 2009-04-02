/* 
   The GE uses memory in 16byte x 8row tiles; swizzling a texture
   makes these tiles linear in memory.

   64x16 byte texture:
   1111 1111 1111 1111  2222 2222 2222 2222  3333 3333 3333 3333  4444 4444 4444 4444
   1111 1111 1111 1111  2222 2222 2222 2222  3333 3333 3333 3333  4444 4444 4444 4444
   1111 1111 1111 1111  2222 2222 2222 2222  3333 3333 3333 3333  4444 4444 4444 4444
   1111 1111 1111 1111  2222 2222 2222 2222  3333 3333 3333 3333  4444 4444 4444 4444
   1111 1111 1111 1111  2222 2222 2222 2222  3333 3333 3333 3333  4444 4444 4444 4444
   1111 1111 1111 1111  2222 2222 2222 2222  3333 3333 3333 3333  4444 4444 4444 4444
   1111 1111 1111 1111  2222 2222 2222 2222  3333 3333 3333 3333  4444 4444 4444 4444
   1111 1111 1111 1111  2222 2222 2222 2222  3333 3333 3333 3333  4444 4444 4444 4444

   5555 5555 5555 5555  6666 6666 6666 6666  7777 7777 7777 7777  8888 8888 8888 8888
   5555 5555 5555 5555  6666 6666 6666 6666  7777 7777 7777 7777  8888 8888 8888 8888
   5555 5555 5555 5555  6666 6666 6666 6666  7777 7777 7777 7777  8888 8888 8888 8888
   5555 5555 5555 5555  6666 6666 6666 6666  7777 7777 7777 7777  8888 8888 8888 8888
   5555 5555 5555 5555  6666 6666 6666 6666  7777 7777 7777 7777  8888 8888 8888 8888
   5555 5555 5555 5555  6666 6666 6666 6666  7777 7777 7777 7777  8888 8888 8888 8888
   5555 5555 5555 5555  6666 6666 6666 6666  7777 7777 7777 7777  8888 8888 8888 8888
   5555 5555 5555 5555  6666 6666 6666 6666  7777 7777 7777 7777  8888 8888 8888 8888

   is swizzled in memory with all '1's contigious, then '2', '3', '4', '5', etc.

   Traversing horizontally (in 16 byte steps), the swizzled output offsets will be:
   0,  128, 256, 384,
   16, 144, 272, 400,
   32, 160, 288, 416...
 */

/*
   Given an offset in a texture 2^log2_w bytes wide, return the byte
   offset in swizzledom.

   Variables are:
   bx,by: coords of 16x8 block within the texture
   mx,my: offsets within the block

   The swizzle function is essentially a 3-bit left-rotate of bits
   4-(log2(width)+7) within the offset.  The by and mx fields are
   unchanged after the operation.

   Unswizzle is a 3-bit right-rotate of the same bitfield.

                                       v lg2(width)
   ...|19|18|17|16|15|14|13|12|11|10| 9| 8| 7| 6| 5| 4| 3| 2| 1| 0|
             ...by by by by by my my my bx bx bx bx bx mx mx mx mx
                               --<--<--<--<--<--<--<-- <rotate
             ...by by by by by bx bx bx bx bx my my my mx mx mx mx
   ...|19|18|17|16|15|14|13|12|11|10| 9| 8| 7| 6| 5| 4| 3| 2| 1| 0|
                              ^lg2(width)+3  ^        ^
                               -->-->-->-->-->-->-->-- >rotate
   ...|19|18|17|16|15|14|13|12|11|10| 9| 8| 7| 6| 5| 4| 3| 2| 1| 0|
             ...by by by by by my my my bx bx bx bx bx mx mx mx mx

 */
unsigned swizzle(unsigned offset, unsigned log2_w)
{
	if (log2_w <= 4)
		return offset;

	unsigned w_mask = (1 << log2_w) - 1;

	unsigned mx = offset & 0xf;
	unsigned by = offset & (~7 << log2_w);
	unsigned bx = offset & w_mask & ~0xf;
	unsigned my = offset & (7 << log2_w);

	return by | (bx << 3) | (my >> (log2_w - 4)) | mx;
}

unsigned unswizzle(unsigned offset, unsigned log2_w)
{
	if (log2_w <= 4)
		return offset;

	unsigned w_mask = (1 << log2_w) - 1;

	unsigned mx = offset & 0xf;
	unsigned by = offset & (~7 << log2_w);
	unsigned bx = offset & ((w_mask & 0xf) << 7);
	unsigned my = offset & 0x70;

	return by | (bx >> 3) | (my << (log2_w - 4)) | mx;
}

#if 1
#include <stdio.h>

extern unsigned char firefox_start[];
extern unsigned char sprite_rgba_start[];

#define SPRITE sprite_rgba_start
#define WIDTH 256
#define HEIGHT 256

typedef unsigned char u8;

void chpswizzle(u8* out, const u8* in, unsigned int width, unsigned int height)
{
   unsigned int i,j;
   unsigned int rowblocks = (width / 16);
 
   for (j = 0; j < height; ++j)
   {
      for (i = 0; i < width; ++i)
      {
         unsigned int blockx = i / 16;
         unsigned int blocky = j / 8;
 
         unsigned int x = (i - blockx*16);
         unsigned int y = (j - blocky*8);
         unsigned int block_index = blockx + ((blocky) * rowblocks);
         unsigned int block_address = block_index * 16 * 8;

         out[block_address + x + y * 16] = in[i+j*width];
      }
   }
}

static
unsigned long lg2 (unsigned long x)
{
	long i;

	for (i=12; i>=0; i--) {
		if ((1 << i) <= x)
			break;
	}

	return i;
}

int main()
{
	unsigned char sw[WIDTH*HEIGHT*4];
	unsigned char unsw[WIDTH*HEIGHT*4];
	int i;
	FILE *fp;
	int lg2w = lg2(WIDTH*4);

#if 1
	for(i = 0; i < WIDTH*HEIGHT*4; i++) {
		unsigned s = swizzle(i, lg2w);
		//printf("swiz %d -> %d\n", i, s);
		sw[s] = SPRITE[i];
	}
#else
	chpswizzle(sw, SPRITE, WIDTH*4, HEIGHT);
#endif
	fp = fopen("swiz.ppm", "wb");

	fprintf(fp, "P6\n%d %d 255\n", WIDTH, HEIGHT);
	for(i = 0; i < WIDTH*HEIGHT*4; i += 4)
		fwrite(&sw[i], 3, 1, fp);

	fclose(fp);

	for(i = 0; i < WIDTH*HEIGHT*4; i++) {
		unsigned u = unswizzle(i, lg2w);
		unsw[u] = sw[i];
	}

	fp = fopen("unswiz.ppm", "wb");
	fprintf(fp, "P6\n%d %d 255\n", WIDTH, HEIGHT);
	for(i = 0; i < WIDTH*HEIGHT*4; i += 4)
		fwrite(&unsw[i], 3, 1, fp);

	fclose(fp);

	return 0;
}
#endif

