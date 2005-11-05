#include <string.h>
#include <psputils.h>

#include "pspgl_internal.h"
#include "pspgl_texobj.h"

static void convert_subimage(struct pspgl_teximg *img, const void *pixels, 
			     int xoffset, int yoffset, unsigned width, unsigned height)
{
	const unsigned pixsz = img->texfmt->hwsize;
	unsigned char *to = img->image + (yoffset * img->stride + xoffset) * pixsz;
	unsigned tostride = img->stride * pixsz;
	unsigned fromstride = width * pixsz;
	void (*convert)(const struct pspgl_texfmt *, void *to, const void *from, unsigned width);
	convert = img->texfmt->convert;

	while(height--) {
		(*convert)(img->texfmt, to, pixels, width);
		to += tostride;
		pixels += fromstride;
	}
}

void glTexSubImage2D( GLenum target, GLint level,
		      GLint xoffset, GLint yoffset,
		      GLsizei width, GLsizei height,
		      GLenum format, GLenum type,
		      const GLvoid *pixels )
{
	struct pspgl_texobj *tobj;
	struct pspgl_teximg *timg;
	const struct pspgl_texfmt *texfmt;

	if (!pspgl_curctx->texture.bound)
		glBindTexture(target, 0);

	tobj = pspgl_curctx->texture.bound;

	if (tobj == NULL)
		goto out_of_memory;

	if (tobj->target != target)
		goto invalid_enum;

	if (tobj->texfmt == NULL)
		goto invalid_operation;

	if (level < 0 || level >= MIPMAP_LEVELS)
		goto invalid_value;

	if (width < 0 || height < 0 || xoffset < 0 || yoffset < 0)
		goto invalid_value;

	timg = tobj->images[level];
	if (timg == NULL)
		goto invalid_operation;

	if (xoffset+width > timg->width || yoffset+height > timg->height)
		goto invalid_value;

	texfmt = __pspgl_hardware_format(__pspgl_texformats, format, type);
	if (texfmt != tobj->texfmt)
		goto invalid_enum;

	/* Can't handle compressed images yet */
	if (texfmt->hwformat >= GE_DXT1 || texfmt->hwformat == GE_INDEX_4BIT)
		goto invalid_operation;

	assert(timg->refcount > 0);

	if (timg->refcount > 1) {
		/* If the timg is currently busy, we can't change it
		   in place, so make a copy before we modify it.

		   XXX This will always happen, because merely binding
		   to a texture will cause its images to be loaded
		   into hardware giving a refcount of at least 2.  We
		   should defer the hardware setup/refcount until a
		   primitive is actually drawn using the texture
		   image.
		*/
		struct pspgl_teximg *timg2;

		timg2 = __pspgl_teximg_new(NULL, timg->width, timg->height, 0, timg->texfmt);

		if (timg2 == NULL)
			goto out_of_memory;
		
		memcpy(timg2->image, timg->image, timg2->size);

		__pspgl_set_texture_image(tobj, level, timg2);
		__pspgl_teximg_free(timg2); /* tobj has reference now */

		timg = timg2;
	}

	convert_subimage(timg, pixels, xoffset, yoffset, width, height);

	sceKernelDcacheWritebackRange(timg->image, timg->size);

	return;

  out_of_memory:
	GLERROR(GL_OUT_OF_MEMORY);
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
}
