#include "pspgl_internal.h"

static
int filter_gl2ge (GLenum f)
{
	if (f >= GL_NEAREST_MIPMAP_NEAREST)
		f += -GL_NEAREST_MIPMAP_NEAREST + GE_TEX_FILTER_NEAREST_MIPMAP_NEAREST;
	return (f & 0x7);
}


static
void update_clamp (struct pspgl_context *c, int shift, GLenum param)
{
	switch (param) {
	case GL_REPEAT:
	case GL_CLAMP:
		__pspgl_context_writereg_masked(c, CMD_TEXWRAP, (param - GL_REPEAT) << shift, 0xff << shift);
		break;
	default:
		GLERROR(GL_INVALID_ENUM);
	}
}


static
void update_texfilter (struct pspgl_context *c, int shift, GLenum param)
{
	switch (param) {
	case GL_NEAREST:
	case GL_LINEAR:
	case GL_NEAREST_MIPMAP_NEAREST:
	case GL_LINEAR_MIPMAP_NEAREST:
	case GL_NEAREST_MIPMAP_LINEAR:
	case GL_LINEAR_MIPMAP_LINEAR:
		__pspgl_context_writereg_masked(c, CMD_TEXFILT, filter_gl2ge(param) << shift, 0xff << shift);
		break;
	default:
		GLERROR(GL_INVALID_ENUM);
	}
}


void glTexParameterfv (GLenum target, GLenum pname, const GLfloat *params)
{
	if (target != GL_TEXTURE_2D)
		goto invalid_enum;

	if (!pspgl_curctx->texture.bound)
		glBindTexture(target, 0);

	switch (pname) {
	case GL_TEXTURE_WRAP_S:
	case GL_TEXTURE_WRAP_T:
		update_clamp(pspgl_curctx, (pname == GL_TEXTURE_WRAP_S) ? 0 : 8, params[0]);
		break;
	case GL_TEXTURE_MAG_FILTER:
	case GL_TEXTURE_MIN_FILTER:
		update_texfilter(pspgl_curctx, (pname == GL_TEXTURE_MIN_FILTER) ? 0 : 8, params[0]);
		break;
	default:
		goto invalid_enum;
	}

	return;

invalid_enum:
	GLERROR(GL_INVALID_ENUM);
}


void glTexParameterf (GLenum target, GLenum pname, GLfloat param)
{
	glTexParameterfv(target, pname, &param);
}


void glTexParameteri (GLenum target, GLenum pname, GLint param)
{
	GLfloat p = param;
	glTexParameterfv(target, pname, &p);
}

