#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <psputils.h>

#include "pspgl_internal.h"
#include "pspgl_texobj.h"


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

static void copy_4444(const struct pspgl_texfmt *fmt, void *to, const void *from, unsigned width)
{
	unsigned short *dest = to;
	const unsigned short *src = from;

	while(width--)
		*dest++ = swizzle_4444(*src++);
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

static void copy_index4(const struct pspgl_texfmt *fmt, void *to, const void *from, unsigned width)
{
	/* 2 pixels/byte */
	memcpy(to, from, width / 2);
}


static const struct pspgl_texfmt texformats[] = {
	/* format       type                        source sz   hwformat      hw size   converter               use tex alpha */
	{ GL_RGB,	GL_UNSIGNED_BYTE,		3,	GE_RGBA_8888, 4,	copy_888_alpha,		GE_TEXENV_RGB  },
	{ GL_RGB,	GL_UNSIGNED_SHORT_5_5_5_1,	2,	GE_RGBA_5551, 2,	copy_5551,		GE_TEXENV_RGB  },
	{ GL_RGB,	GL_UNSIGNED_SHORT_5_6_5,	2,	GE_RGB_565,   2,	copy_565,		GE_TEXENV_RGB  },
	{ GL_RGB,	GL_UNSIGNED_SHORT_4_4_4_4,	2,	GE_RGBA_4444, 2,	copy_4444,		GE_TEXENV_RGB  },

	{ GL_RGB,	GL_UNSIGNED_SHORT_1_5_5_5_REV,	2,	GE_RGBA_5551, 2,	copy,			GE_TEXENV_RGB  },
	{ GL_RGB,	GL_UNSIGNED_SHORT_5_6_5_REV,	2,	GE_RGB_565,   2,	copy,			GE_TEXENV_RGB  },
	{ GL_RGB,	GL_UNSIGNED_SHORT_4_4_4_4_REV,	2,	GE_RGBA_4444, 2,	copy,			GE_TEXENV_RGB  },

	{ GL_BGR,	GL_UNSIGNED_SHORT_5_6_5,	2,	GE_RGB_565,   2,	copy,			GE_TEXENV_RGB  },

	{ GL_RGBA,	GL_UNSIGNED_BYTE,		4,	GE_RGBA_8888, 4,	copy,			GE_TEXENV_RGBA },
	{ GL_RGBA,	GL_UNSIGNED_SHORT_5_5_5_1,	2,	GE_RGBA_5551, 2,	copy_5551,		GE_TEXENV_RGBA },
	{ GL_RGBA,	GL_UNSIGNED_SHORT_4_4_4_4,	2,	GE_RGBA_4444, 2,	copy_4444,		GE_TEXENV_RGBA },

	{ GL_RGBA,	GL_UNSIGNED_SHORT_1_5_5_5_REV,	2,	GE_RGBA_5551, 2,	copy,			GE_TEXENV_RGBA },
	{ GL_RGBA,	GL_UNSIGNED_SHORT_4_4_4_4_REV,	2,	GE_RGBA_4444, 2,	copy,			GE_TEXENV_RGBA },

	{ GL_ABGR_EXT,	GL_UNSIGNED_SHORT_4_4_4_4,	2,	GE_RGBA_4444, 2,	copy,			GE_TEXENV_RGBA },

	{ GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE,		2,	GE_RGBA_8888, 4,	copy_expand_LA,		GE_TEXENV_RGBA },

	/* XXX These two could use an indexed mode to save space */
	{ GL_LUMINANCE,	GL_UNSIGNED_BYTE,		1,	GE_RGBA_8888, 4,	copy_expand_L,		GE_TEXENV_RGB  },
	{ GL_ALPHA,	GL_UNSIGNED_BYTE,		1,	GE_RGBA_8888, 4,	copy_expand_A,		GE_TEXENV_RGBA },
#if 0
	/* an intensity (L==A) format with efficent (1 byte/texel) storage is very useful for fonts */
	{ GL_INTENSITY,	GL_UNSIGNED_BYTE,		1,	GE_INDEX_8BIT, 1,	xxx,			GE_TEXENV_RGBA },
#endif

 	/* Indexed textures */
 	{ GL_COLOR_INDEX4_EXT,	GL_UNSIGNED_BYTE,	0,	GE_INDEX_4BIT, 0,	copy_index4,		0 /* ? */},
 	{ GL_COLOR_INDEX8_EXT,	GL_UNSIGNED_BYTE,	1,	GE_INDEX_8BIT, 1,	copy,			0 /* ? */},
 	{ GL_COLOR_INDEX16_EXT,	GL_UNSIGNED_SHORT,	2,	GE_INDEX_16BIT, 2,	copy,			0 /* ? */},
};
#define NTEXFORMATS	(sizeof(texformats)/sizeof(*texformats))

const struct pspgl_texfmt *__pspgl_hardware_format(GLenum format, GLenum type)
{
	int i;

	for(i = 0; i < NTEXFORMATS; i++)
		if (texformats[i].format == format &&
		    texformats[i].type == type)
			return &texformats[i];

	return NULL;
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


static void tobj_setstate(struct pspgl_texobj *tobj, unsigned reg, unsigned setting)
{
	assert(reg >= TEXSTATE_START && reg <= TEXSTATE_END);

	tobj->ge_texreg[reg - TEXSTATE_START] = (reg << 24) | (setting & 0xffffff);
}


struct pspgl_texobj *__pspgl_texobj_new(GLint id, GLenum target)
{
	struct pspgl_texobj *tobj = malloc(sizeof(*tobj));
	unsigned i;

	memset(tobj, 0, sizeof(*tobj));

	tobj->refcount = 1;
	tobj->target = target;

	for(i = TEXSTATE_START; i <= TEXSTATE_END; i++)
		tobj_setstate(tobj, i, 0);

	tobj_setstate(tobj, CMD_TEXFILT, (GE_TEX_FILTER_LINEAR << 8) | GE_TEX_FILTER_LINEAR);
	tobj_setstate(tobj, CMD_TEXWRAP, (GE_TEX_WRAP_REPEAT << 8) | GE_TEX_WRAP_REPEAT);

	return tobj;
}

void __pspgl_texobj_free(struct pspgl_texobj *tobj)
{
	int i;

	if (--tobj->refcount)
		return;

	for(i = 0; i < MIPMAP_LEVELS; i++)
		if (tobj->images[i] != NULL)
			__pspgl_teximg_free(tobj->images[i]);

	if (tobj->cmap != NULL)
		__pspgl_teximg_free(tobj->cmap);

	free(tobj);
}


struct pspgl_teximg *__pspgl_teximg_new(const void *pixels, int width, int height,
					const struct pspgl_texfmt *texfmt)
{
	struct pspgl_teximg *timg;
	unsigned size;

	timg = malloc(sizeof(*timg));
	if (timg == NULL)
 		return NULL;

	memset(timg, 0, sizeof(*timg));
	timg->refcount = 1;

	size = width * height * texfmt->hwsize;

	/* XXX try to allocate edram? */
	timg->image = memalign(16, size);

	if (!timg->image) {
		free(timg);
		return NULL;
	}

	__pspgl_convert_image(pixels, width, height, timg->image, texfmt);

	/* Make texture data visible to hardware */
	sceKernelDcacheWritebackRange(timg->image, size);

	timg->width = width;
	timg->height = height;
	timg->stride = width;
	timg->texfmt = texfmt;

	return timg;
}

void __pspgl_teximg_free(struct pspgl_teximg *timg)
{
	if (--timg->refcount)
		return;

	free(timg->image);
	free(timg);
}

static GLboolean texfmt_is_indexed(const struct pspgl_texfmt *fmt)
{
	return fmt->hwformat >= GE_INDEX_4BIT && fmt->hwformat <= GE_INDEX_32BIT;
}


void __pspgl_update_texenv(struct pspgl_texobj *tobj)
{
	const struct pspgl_texfmt *fmt;

	if (tobj == NULL)
		return;

	fmt = tobj->texfmt;
	if (fmt && texfmt_is_indexed(fmt) && tobj->cmap != NULL)
		fmt = tobj->cmap->texfmt;

	if (fmt)
		__pspgl_context_writereg_masked(pspgl_curctx, CMD_TEXENV_FUNC, fmt->texalpha, 0x100);
}
