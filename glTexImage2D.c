#include "pspgl_internal.h"


#define GE_RGB_565     0
#define GE_RGBA_5551   1
#define GE_RGBA_4444   2
#define GE_RGBA_8888   3
#define GE_INDEX_4BIT  4
#define GE_INDEX_8BIT  5
#define GE_INDEX_16BIT 6
#define GE_INDEX_32BIT 7

static
unsigned long lg2 (unsigned long x)
{
	long i;

	for (i=9; i>=0; i--) {
		if ((1 << i) <= x)
			break;
	}

	return i;
}


void glTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *texels)
{
	unsigned long pixfmt;
	unsigned long log2width = lg2(width);
	unsigned long log2height = lg2(height);

	if ((1 << log2width) != width || (1 << log2height) != height)
		goto invalid_value;

	if (target != GL_TEXTURE_2D)
		goto invalid_enum;

	if (level < 0 || level > 7)
		goto invalid_value;

	switch (format) {
	case GL_RGB:
		if (type != GL_UNSIGNED_SHORT_5_6_5)
			goto invalid_enum;
		pixfmt = GE_RGB_565;
		break;
	case GL_RGBA:
		switch (type) {
		case GL_UNSIGNED_SHORT_5_5_5_1:
			pixfmt = GE_RGBA_5551;
			break;
		case GL_UNSIGNED_SHORT_4_4_4_4:
			pixfmt = GE_RGBA_4444;
			break;
		case GL_UNSIGNED_INT_8_8_8_8:
			pixfmt = GE_RGBA_8888;
			break;
		default:
			goto invalid_enum;
		}
		break;
	case GL_COLOR_INDEX:
		switch (type) {
		case GL_UNSIGNED_BYTE:
			pixfmt = GE_INDEX_8BIT;
			break;
		case GL_UNSIGNED_SHORT:
			pixfmt = GE_INDEX_16BIT;
			break;
		case GL_UNSIGNED_INT:
			pixfmt = GE_INDEX_32BIT;
			break;
		default:
			goto invalid_enum;
		}
		break;
	default:
		goto invalid_enum;
	}

	sendCommandi(194, 0);
	sendCommandi(195, pixfmt);

	sendCommandi(160 + level, ((unsigned long) texels) & 0xffffff);
	sendCommandi(168 + level, ((((unsigned long) texels) >> 8) & 0x0f0000) | width);
	sendCommandi(184 + level, (log2height << 8) | log2width);

	/* Texture Flush */
	sendCommandi(203, 0);
	return;

invalid_enum:
	GLERROR(GL_INVALID_ENUM);
	return;

invalid_value:
	GLERROR(GL_INVALID_VALUE);
}
