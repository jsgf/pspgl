#include <string.h>

#include "pspgl_internal.h"
#include "pspgl_texobj.h"

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

static inline unsigned ispow2(unsigned n)
{
	return (n & (n-1)) == 0;
}


/*
  RRRRRGGGGGGBBBBB	<- GL
  BBBBBGGGGGGRRRRR	<- PSP
 */
static inline unsigned short swizzle_565(unsigned short in)
{
	unsigned short r = (in & 0xf800) >> 11;
	unsigned short g = (in & 0x07e0);
	unsigned short b = (in & 0x001f) << 11;

	return b | g | r;
}

/*
  RRRRRGGGGGBBBBBA	<- GL
  ABBBBBGGGGGRRRRR	<- PSP
 */
static inline unsigned short swizzle_5551(unsigned short in)
{
	unsigned short r = (in & 0xf800) >> 11;
	unsigned short g = (in & 0x07c0) >> 1;
	unsigned short b = (in & 0x0036) << 9;
	unsigned short a = (in & 0x0001) << 15;

	return a | b | g | r;
}

/*
  RRRRGGGGBBBBAAAA	<- GL
  AAAABBBBGGGGRRRR	<- PSP
 */
static inline unsigned short swizzle_4444(unsigned short in)
{
	unsigned short r = (in & 0xf000) >> 12;
	unsigned short g = (in & 0x0f00) >> 4;
	unsigned short b = (in & 0x00f0) << 4;
	unsigned short a = (in & 0x000f) << 12;

	return a | b | g | r;
}

static void copy(const struct pspgl_texfmt *fmt, void *to, const void *from, unsigned width)
{
	memcpy(to, from, width * fmt->pixsize);
}

static void copy_565(const struct pspgl_texfmt *fmt, void *to, const void *from, unsigned width)
{
	unsigned short *dest = to;
	const unsigned short *src = from;

	while(width--)
		*dest++ = swizzle_565(*src++);
}

static void copy_5551(const struct pspgl_texfmt *fmt, void *to, const void *from, unsigned width)
{
	unsigned short *dest = to;
	const unsigned short *src = from;

	while(width--)
		*dest++ = swizzle_5551(*src++);
}

static void copy_5551_alpha(const struct pspgl_texfmt *fmt, void *to, const void *from, unsigned width)
{
	unsigned short *dest = to;
	const unsigned short *src = from;

	while(width--)
		*dest++ = swizzle_5551(*src++ | 0x0001); /* insert alpha */
}

static void copy_4444(const struct pspgl_texfmt *fmt, void *to, const void *from, unsigned width)
{
	unsigned short *dest = to;
	const unsigned short *src = from;

	while(width--)
		*dest++ = swizzle_4444(*src++);
}

static void copy_4444_alpha(const struct pspgl_texfmt *fmt, void *to, const void *from, unsigned width)
{
	unsigned short *dest = to;
	const unsigned short *src = from;

	while(width--)
		*dest++ = swizzle_4444(*src++ | 0x000f); /* insert alpha */
}

static void copy_888_alpha(const struct pspgl_texfmt *fmt, void *to, const void *from, unsigned width)
{
	unsigned char *dest = to;
	const unsigned char *src = from;

	while(width--) {
		dest[0] = src[0];
		dest[1] = src[1];
		dest[2] = src[2];
		dest[3] = 0xff;

		dest += 4;
		src += 3;
	}
}

static void copy_expand_LA(const struct pspgl_texfmt *fmt, void *to, const void *from, unsigned width)
{
	unsigned int *dest = to;
	const unsigned char *src = from;

	while(width--) {
		unsigned char l = src[0];
		unsigned char a = src[1];

		*dest = (a << 24) | (l << 16) | (l << 8) | l;

		dest++;
		src += 2;
	}
}

static void copy_expand_A(const struct pspgl_texfmt *fmt, void *to, const void *from, unsigned width)
{
	unsigned int *dest = to;
	const unsigned char *src = from;

	while(width--)
		*dest++ = (*src++) << 24;
}

static void copy_expand_L(const struct pspgl_texfmt *fmt, void *to, const void *from, unsigned width)
{
	unsigned int *dest = to;
	const unsigned char *src = from;

	while(width--) {
		unsigned char l = *src++;

		*dest++ = 0xff000000 | (l << 16) | (l << 8) | l;
	}
}


static const struct pspgl_texfmt texformats[] = {
	/* format       type                        source sz   hwformat      hw size   converter               use tex alpha */
	{ GL_RGB,	GL_UNSIGNED_BYTE,		3,	GE_RGBA_8888, 4,	copy_888_alpha,		GE_TEXENV_RGB  },
	{ GL_RGB,	GL_UNSIGNED_SHORT_5_5_5_1,	2,	GE_RGBA_5551, 2,	copy_5551_alpha,	GE_TEXENV_RGB  },
	{ GL_RGB,	GL_UNSIGNED_SHORT_5_6_5,	2,	GE_RGB_565,   2,	copy_565,		GE_TEXENV_RGB  },
	{ GL_RGB,	GL_UNSIGNED_SHORT_4_4_4_4,	2,	GE_RGBA_4444, 2,	copy_4444_alpha,	GE_TEXENV_RGB  },

	{ GL_RGBA,	GL_UNSIGNED_BYTE,		4,	GE_RGBA_8888, 4,	copy,			GE_TEXENV_RGBA },
	{ GL_RGBA,	GL_UNSIGNED_SHORT_5_5_5_1,	2,	GE_RGBA_5551, 2,	copy_5551,		GE_TEXENV_RGBA },
	{ GL_RGBA,	GL_UNSIGNED_SHORT_4_4_4_4,	2,	GE_RGBA_4444, 2,	copy_4444,		GE_TEXENV_RGBA },

	{ GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE,		2,	GE_RGBA_8888, 4,	copy_expand_LA,		GE_TEXENV_RGBA },

	/* XXX These two could use an indexed mode to save space */
	{ GL_LUMINANCE,	GL_UNSIGNED_BYTE,		1,	GE_RGBA_8888, 4,	copy_expand_L,		GE_TEXENV_RGB  },
	{ GL_ALPHA,	GL_UNSIGNED_BYTE,		1,	GE_RGBA_8888, 4,	copy_expand_A,		GE_TEXENV_RGBA },
#if 0
	/* an intensity (L==A) format with efficent (1 byte/texel) storage is very useful for fonts */
	{ GL_INTENSITY,	GL_UNSIGNED_BYTE,		1,	GE_INDEX_8BIT, 1,	xxx,			GE_TEXENV_RGBA },
#endif

#if 0
	{ GL_COMPRESSED_RGB_S3TC_DXT1_EXT,	GL_UNSIGNED_BYTE,	1, GE_DXT1,	1,	copy,		GE_TEXENV_RGB  },
	{ GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,	GL_UNSIGNED_BYTE,	1, GE_DXT1,	1,	copy,		GE_TEXENV_RGBA },
	{ GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,	GL_UNSIGNED_BYTE,	1, GE_DXT3,	1,	copy,		GE_TEXENV_RGBA },
	{ GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,	GL_UNSIGNED_BYTE,	1, GE_DXT5,	1,	copy,		GE_TEXENV_RGBA },
#endif
};
#define NTEXFORMATS	(sizeof(texformats)/sizeof(*texformats))

static const struct pspgl_texfmt *hardware_format(GLenum format, GLenum type)
{
	int i;

	for(i = 0; i < NTEXFORMATS; i++)
		if (texformats[i].format == format &&
		    texformats[i].type == type)
			return &texformats[i];

	return NULL;
}

static void set_mipmap_regs(unsigned level, struct pspgl_teximg *img)
{
	if (img) {
		unsigned ptr = (unsigned)img->image;
		unsigned w_lg2 = lg2(img->width);
		unsigned h_lg2 = lg2(img->height);

		/* XXX pin image while texture registers refer to it */

		psp_log("set level %d image=%p %dx%d stride=%d\n",
			level, img->image, img->width, img->height,
			img->stride);

		sendCommandi(CMD_TEX_MIPMAP0 + level, ptr);
		sendCommandi(CMD_TEX_STRIDE0 + level, ((ptr >> 8) & 0xf0000) | img->stride);
		sendCommandi(CMD_TEX_SIZE0 + level, (h_lg2 << 8) | w_lg2);
	} else {
		psp_log("set level %d image=NULL", level);

		sendCommandi(CMD_TEX_MIPMAP0 + level, 0);
		sendCommandi(CMD_TEX_STRIDE0 + level, 0);
		sendCommandi(CMD_TEX_SIZE0 + level, 0);
	}
}

static void set_texture_image(struct pspgl_texobj *tobj, unsigned level, struct pspgl_teximg *timg)
{
	if (tobj->images[level] != NULL) {
		psp_log("replacing texture image %p at level %d with %p\n",
			tobj->images[level], level, timg);
		__pspgl_teximg_free(tobj->images[level]);
	}

	if (timg) {
		timg->refcount++;

		/* if we're changing texture formats, then invalidate all the other images */
		if (tobj->texfmt != timg->texfmt) {
			int i;
			for(i = 0; i < MIPMAP_LEVELS; i++)
				if (tobj->images[i] &&
				    tobj->images[i]->texfmt != timg->texfmt) {
					__pspgl_teximg_free(tobj->images[i]);
					tobj->images[i] = NULL;
					set_mipmap_regs(i, NULL);
				}
		}
		tobj->texfmt = timg->texfmt;

		sendCommandi(CMD_TEXFMT, tobj->texfmt->hwformat);
	}
	tobj->images[level] = timg;
	set_mipmap_regs(level, timg);
}

void __pspgl_convert_image(const void *pixels, int width, int height,
			   void *to, const struct pspgl_texfmt *texfmt)
{
	const unsigned char *src = pixels;
	unsigned char *dest = to;

	psp_log("convert %p -> %p size=%dx%d texfmt=%p (%x/%x -> %d)\n",
		pixels, to, width, height,
		texfmt, texfmt->format, texfmt->type, texfmt->hwformat);

	while(height--) {
		(*texfmt->convert)(texfmt, dest, src, width);
		src += texfmt->pixsize * width;
		dest += texfmt->hwsize * width;
	}
}

static inline GLboolean mipmap_filter(GLenum filter)
{
	return filter >= GL_NEAREST_MIPMAP_NEAREST && filter <= GL_LINEAR_MIPMAP_LINEAR;
}

#if 0
static GLboolean texobj_is_complete(struct pspgl_texobj *tobj)
{
	if (tobj == NULL) {
		psp_log("incomplete: tobj NULL\n");
		return GL_FALSE;
	}

	if (tobj->images[0] == NULL) {
		psp_log("incomplete: base NULL\n");
		return GL_FALSE;
	}

	if (mipmap_filter(tobj->min_filter)) {
		int i;

		/* XXX check sizes too */
		for(i = 1; i < MIPMAP_LEVELS; i++) {
			if (tobj->images[i] == NULL) {
				psp_log("incomplete: mipmap level %d missing\n", i);
				return GL_FALSE;
			}

			if (tobj->images[i]->texfmt != tobj->texfmt) {
				psp_log("incomplete: level %s mismatched format %d %d\n",
					i, tobj->images[i]->texfmt->hwformat, tobj->texfmt->hwformat);
				return GL_FALSE;
			}
		}
	}

	return GL_TRUE;
}
#endif

void glTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *texels)
{
	struct pspgl_texobj *tobj;
	struct pspgl_teximg *timg;
	const struct pspgl_texfmt *texfmt;

	if (!ispow2(width) || !ispow2(height))
		goto invalid_value;

	if (level < 0 || level > MIPMAP_LEVELS)
		goto invalid_value;

	if (border != 0)
		goto invalid_value;

	if (format != internalformat)
		goto invalid_operation;

	texfmt = hardware_format(internalformat, type);
	if (texfmt == NULL)
		goto invalid_enum;

	psp_log("selected texfmt %d for fmt=%x type=%x\n", texfmt->hwformat, internalformat, type);

	if (!pspgl_curctx->texture.bound)
		glBindTexture(target, 0);

	tobj = pspgl_curctx->texture.bound;
	if (tobj == NULL)
		goto out_of_memory;

	if (texels) {
		timg = __pspgl_teximg_new(texels, width, height, texfmt);
		if (timg == NULL)
			goto out_of_memory;

		set_texture_image(tobj, level, timg);
		__pspgl_teximg_free(timg); /* tobj has reference now */
	} else
		set_texture_image(tobj, level, NULL);

	sendCommandi(CMD_TEXCACHE_FLUSH, getReg(CMD_TEXCACHE_FLUSH)+1);
	__pspgl_context_writereg_masked(pspgl_curctx, CMD_TEXENV_FUNC, texfmt->texalpha, 0x100);

	return;

invalid_enum:
	GLERROR(GL_INVALID_ENUM);
	return;

invalid_value:
	GLERROR(GL_INVALID_VALUE);
	return;

invalid_operation:
	GLERROR(GL_INVALID_OPERATION);
	return;

out_of_memory:
	GLERROR(GL_OUT_OF_MEMORY);
}
