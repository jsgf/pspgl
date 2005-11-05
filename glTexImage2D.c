#include <string.h>
#include <malloc.h>

#include "pspgl_internal.h"
#include "pspgl_texobj.h"

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

static void set_mipmap_regs(unsigned level, struct pspgl_teximg *img)
{
	if (img) {
		unsigned ptr = (unsigned)img->image->base + img->offset;
		unsigned w_lg2 = lg2(img->width);
		unsigned h_lg2 = lg2(img->height);

		psp_log("set level %d image=%p %dx%d\n",
			level, img->image, img->width, img->height);

		sendCommandi(CMD_TEX_MIPMAP0 + level, ptr);
		sendCommandi(CMD_TEX_STRIDE0 + level, ((ptr >> 8) & 0xf0000) | img->width);
		sendCommandi(CMD_TEX_SIZE0 + level, (h_lg2 << 8) | w_lg2);
	} else {
		psp_log("set level %d image=NULL", level);

		sendCommandi(CMD_TEX_MIPMAP0 + level, 0);
		sendCommandi(CMD_TEX_STRIDE0 + level, 0);
		sendCommandi(CMD_TEX_SIZE0 + level, 0);
	}
}

void __pspgl_set_texture_image(struct pspgl_texobj *tobj, unsigned level, struct pspgl_teximg *timg)
{
	struct pspgl_teximg *old_timg = tobj->images[level];

	if (timg == old_timg)
		return;

	tobj->images[level] = NULL;

	if (timg) {
		/* if we're changing texture formats, then invalidate all the other images */
		if (tobj->texfmt != timg->texfmt) {
			int i;
			for(i = 0; i < MIPMAP_LEVELS; i++) {
				if (i == level)
					continue;

				if (tobj->images[i] && 
				    tobj->images[i]->texfmt != timg->texfmt) {
					__pspgl_teximg_free(tobj->images[i]);
					tobj->images[i] = NULL;
					set_mipmap_regs(i, NULL);
				}
			}
		}
		tobj->texfmt = timg->texfmt;

		sendCommandi(CMD_TEXFMT, tobj->texfmt->hwformat);
	}
	tobj->images[level] = timg;
	set_mipmap_regs(level, timg);

	if (old_timg != NULL) {
		psp_log("replaced texture image %p at level %d with %p\n", 
			old_timg, level, timg);

		/* release the tobj's reference to the old texture image */
		__pspgl_teximg_free(old_timg);
	}

	sendCommandi(CMD_TEXCACHE_FLUSH, getReg(CMD_TEXCACHE_FLUSH)+1);
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

	if (texfmt_is_indexed(tobj->texfmt) && tobj->cmap == NULL)
		return GL_FALSE;

	return GL_TRUE;
}
#endif

void glTexImage2D (GLenum target, GLint level, GLint internalformat, 
		   GLsizei width, GLsizei height, GLint border, 
		   GLenum format, GLenum type, const GLvoid *texels)
{
	static const GLenum format_equiv[] = {
		0,
		GL_LUMINANCE,
		GL_LUMINANCE_ALPHA,
		GL_RGB,
		GL_RGBA,
	};

	struct pspgl_texobj *tobj;
	struct pspgl_teximg *timg;
	const struct pspgl_texfmt *texfmt;

	if (target != GL_TEXTURE_2D)
		goto invalid_enum;

	if (!ispow2(width) || !ispow2(height))
		goto invalid_value;

	if (level < 0 || level > MIPMAP_LEVELS)
		goto invalid_value;

	if (border != 0)
		goto invalid_value;

	/* old-style formats are just number of components */
	if (internalformat <= 4 && internalformat > 0)
		internalformat = format_equiv[internalformat];

	if (format != internalformat)
		goto invalid_operation;

	texfmt = __pspgl_hardware_format(__pspgl_texformats, internalformat, type);
	if (texfmt == NULL)
		goto invalid_enum;

	if (texfmt->hwformat >= GE_DXT1)
		goto invalid_enum;

	psp_log("selected texfmt %d for fmt=%x type=%x\n", texfmt->hwformat, internalformat, type);

	if (!pspgl_curctx->texture.bound)
		glBindTexture(target, 0);

	tobj = pspgl_curctx->texture.bound;
	if (tobj == NULL)
		goto out_of_memory;

	timg = __pspgl_teximg_new(texels, pspgl_curctx->texture.unpackbuffer, width, height, 0, texfmt);
	if (timg == NULL)
		goto out_of_memory;

	__pspgl_set_texture_image(tobj, level, timg);

	__pspgl_update_texenv(tobj);
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
