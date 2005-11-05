#include <string.h>
#include <psputils.h>

#include "pspgl_internal.h"
#include "pspgl_texobj.h"

static void convert_subimage(struct pspgl_teximg *img, const void *pixels, 
			     int xoffset, int yoffset, unsigned width, unsigned height)
{
	const unsigned pixsz = img->texfmt->hwsize;
	unsigned char *to = img->image.base + (yoffset * img->width + xoffset) * pixsz;
	unsigned tostride = img->width * pixsz;
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

	assert(timg->image.refcount > 0);

	if (timg->image.refcount > 1) {
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

		__pspgl_buffer_map(&timg->image, GL_READ_ONLY_ARB);
		__pspgl_buffer_map(&timg2->image, GL_WRITE_ONLY_ARB);
		
		memcpy(timg2->image.base, timg->image.base, timg2->image.size);

		__pspgl_set_texture_image(tobj, level, timg2);
		__pspgl_teximg_free(timg2); /* tobj has reference now */

		__pspgl_buffer_unmap(&timg->image, GL_READ_ONLY_ARB);

		timg = timg2;
	} else
		__pspgl_buffer_map(&timg->image, GL_WRITE_ONLY_ARB);

	convert_subimage(timg, pixels, xoffset, yoffset, width, height);

	__pspgl_buffer_unmap(&timg->image, GL_WRITE_ONLY_ARB);

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
