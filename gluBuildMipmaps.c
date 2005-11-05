#include <GL/glu.h>
#include <GL/gl.h>
#include <stdlib.h>
#include <string.h>

#include "pspgl_texobj.h"

/* 
   All completely untested.  Mostly junk.
 */

static unsigned pow2(unsigned x)
{
	unsigned ret = 1;

	while(ret < x)
		ret <<= 1;

	return ret;
}

static inline unsigned fetchpix(const void *base, GLsizei width, GLsizei height,
				const struct pspgl_texfmt *fmt,
				unsigned x, unsigned y)
{
	const unsigned char *addr = base + (y*width + x) * fmt->pixsize;
	unsigned char r,g,b,a;

	switch(fmt->hwformat) {
	case GE_RGBA_8888:
		r = addr[3];
		g = addr[2];
		b = addr[1];
		a = addr[0];
		break;

	case GE_RGBA_4444: {
		/* aaaa bbbb gggg rrrr */
		unsigned short t = *(unsigned short *)addr;
		r = ((t & 0x000f) >>  0) * 255 / 15;
		g = ((t & 0x00f0) >>  4) * 255 / 15;
		b = ((t & 0x0f00) >>  8) * 255 / 15;
		a = ((t & 0xf000) >> 12) * 255 / 15;
		break;
	}
	case GE_RGBA_5551: {
		/* abbb bbgg gggr rrrr */
		unsigned short t = *(unsigned short *)addr;
		r = ((t & 0x001f) >>  0) * 255 / 31;
		g = ((t & 0x03e0) >>  5) * 255 / 31;
		b = ((t & 0x7c00) >> 10) * 255 / 31;
		a = ((t & 0x8000) >> 15) * 255 / 1;
		break;
	}

	case GE_RGB_565: {
		/* bbbb bggg gggr rrrr */
		unsigned short t = *(unsigned short *)addr;
		r = ((t & 0x001f) >>  0) * 255 / 31;
		g = ((t & 0x7e00) >>  5) * 255 / 63;
		b = ((t & 0xf800) >>  9) * 255 / 31;
		a = 255;
		break;
	}

	default:
		return 0;
	}

	return (a << 24) | (b << 16) | (g << 8) | r;
}

static inline void putpix(void *base, unsigned width, unsigned height, 
			  const struct pspgl_texfmt *fmt, unsigned x, unsigned y,
			  unsigned pix)
{
	const unsigned char *addr = base + (y*width + x) * fmt->pixsize;

	switch(fmt->hwformat) {
	case GE_RGBA_8888:
		*(unsigned *)addr = pix;
		break;

	case GE_RGBA_5551:
	case GE_RGBA_4444:
	case GE_RGB_565:
		*(unsigned short *)addr = pix;
		break;
	}
}


static void *rescale_width(GLsizei width, GLsizei height, 
			   const struct pspgl_texfmt *fmt, const void *data)
{
	GLsizei width2 = pow2(width);
	unsigned scale;
	void *output;
	int y;

	if (width2 > 512)
		width2 = 512;

	scale = width * 65536 / width2;

	output = malloc(width2 * height * fmt->pixsize);
	if (output == NULL)
		return NULL;

	for(y = 0; y < height; y++) {
		unsigned x, srcx;
		for(x = srcx = 0; x < width2; x++, srcx += scale) {
			unsigned pix = fetchpix(data, width, height, fmt, (srcx >> 16), y);

			putpix(output, width2, height, fmt, x, y, pix);
		}
	}

	return output;
}

static void *rescale_height(GLsizei width, GLsizei height, const struct pspgl_texfmt *fmt,
			    const void *data)
{
	unsigned scale;
	void *output;
	unsigned y, srcy;
	unsigned height2 = pow2(height);

	if (height2 > 512)
		height2 = 512;

	scale = height * 65536 / height2;

	output = malloc(width * height2 * fmt->pixsize);
	if (output == NULL)
		return NULL;

	for(srcy = y = 0; y < height2; y++, srcy += scale) {
		unsigned x;
		for(x = 0; x < width; x++) {
			unsigned pix = fetchpix(data, width, height, fmt, x, (srcy >> 16));

			putpix(output, width, height2, fmt, x, y, pix);
		}
	}

	return output;
}

GLint gluBuild2DMipmaps( GLenum target,
			 GLint internalFormat,
			 GLsizei width,
			 GLsizei height,
			 GLenum format,
			 GLenum type,
			 const void *data )
{
	const void *image = data;
	const struct pspgl_texfmt *fmt = __pspgl_hardware_format(__pspgl_texformats, format, type);

	if (fmt == NULL)
		return GLU_INVALID_ENUM;

	if (width != pow2(width)) {
		image = rescale_width(width, height, fmt, image);
		if (image == NULL)
			return GLU_OUT_OF_MEMORY;
		width = pow2(width);
	}

	if (height != pow2(height)) {
		void *t = rescale_height(width, height, fmt, image);

		if (t == NULL)
			return GLU_OUT_OF_MEMORY;

		if (image != data)
			free(image);
		image = t;

		height = pow2(height);
	}

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, image);

	if (image != data)
		free(image);

	return 0;
}
