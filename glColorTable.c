#include "pspgl_internal.h"
#include "pspgl_texobj.h"

void glColorTable(GLenum target, GLenum internalformat, 
		  GLsizei width, GLenum format, GLenum type, const GLvoid *data)
{
	struct pspgl_teximg *cmap;
	struct pspgl_texobj *tobj;
	const struct pspgl_texfmt *fmt;

	if (target != GL_TEXTURE_2D)
		goto invalid_enum;

	if (internalformat != format)
		goto invalid_operation;

	if (!(format == GL_RGB || format == GL_RGBA))
		goto invalid_enum;

	if (!ispow2(width))
		goto invalid_value;

	fmt = __pspgl_hardware_format(__pspgl_texformats, format, type);
	if (fmt == 0)
		goto invalid_enum;

	cmap = __pspgl_teximg_new(data, pspgl_curctx->texture.unpackbuffer, width, 1, 0, fmt);
	if (cmap == 0)
		goto out_of_memory;

	if (!pspgl_curctx->texture.bound)
		glBindTexture(target, 0);

	tobj = pspgl_curctx->texture.bound;

	if (tobj->cmap) {
		/* release old cmap */
		__pspgl_teximg_free(tobj->cmap);
	}
	tobj->cmap = cmap;

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
