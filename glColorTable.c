#include "pspgl_internal.h"
#include "pspgl_texobj.h"

void glColorTable(GLenum target, GLenum internalformat, 
		  GLsizei width, GLenum format, GLenum type, const GLvoid *data)
{
	struct pspgl_teximg *cmap;
	struct pspgl_texobj *tobj;
	const struct pspgl_texfmt *fmt;
	unsigned long p;

	if (target != GL_TEXTURE_2D)
		goto invalid_enum;

	if (internalformat != format)
		goto invalid_operation;

	if (!(format == GL_RGB || format == GL_RGBA))
		goto invalid_enum;

	if (!ispow2(width))
		goto invalid_value;

	fmt = __pspgl_hardware_format(format, type);
	if (fmt == 0)
		goto invalid_enum;

	cmap = __pspgl_teximg_new(data, width, 1, fmt);
	if (cmap == 0)
		goto out_of_memory;

	if (!pspgl_curctx->texture.bound)
		glBindTexture(target, 0);

	tobj = pspgl_curctx->texture.bound;

	if (tobj->cmap != 0)
		__pspgl_teximg_free(tobj->cmap);
	tobj->cmap = cmap;

	p = (unsigned long)cmap->image;

	/* XXX pin clut while pending primitives are still using it */
	sendCommandi(CMD_SET_CLUT, p);
	sendCommandi(CMD_SET_CLUT_MSB, (p >> 8) & 0xf0000);
	/* Not sure what the 0xff << 8 is about, but
	   samples/gu/blend.c uses it, and it seems to be
	   necessary to get a non-black output... */
	sendCommandi(CMD_CLUT_MODE, cmap->texfmt->hwformat | (0xff << 8));
	sendCommandi(CMD_CLUT_BLKS, cmap->width / 8);

	__pspgl_update_texenv(tobj);

	return;

  invalid_enum:
	GLERROR(GL_INVALID_ENUM);
	return;

  invalid_operation:
	GLERROR(GL_INVALID_OPERATION);
	return;

  invalid_value:
	GLERROR(GL_INVALID_VALUE);
	return;

  out_of_memory:
	GLERROR(GL_OUT_OF_MEMORY);
	return;
}

void glColorTableEXT(GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *data)
	__attribute__((alias("glColorTable")));
