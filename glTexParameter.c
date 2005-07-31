#include "pspgl_internal.h"


#define GE_TEX_WRAP_REPEAT	0
#define GE_TEX_WRAP_CLAMP	1

#define GE_TEX_FILTER_NEAREST			0
#define GE_TEX_FILTER_LINEAR			1
#define GE_TEX_FILTER_NEAREST_MIPMAP_NEAREST	4
#define GE_TEX_FILTER_LINEAR_MIPMAP_NEAREST	5
#define GE_TEX_FILTER_NEAREST_MIPMAP_LINEAR	6
#define GE_TEX_FILTER_LINEAR_MIPMAP_LINEAR	7


static
int filter_gl2ge (GLenum f)
{
	if (f >= GL_NEAREST_MIPMAP_NEAREST)
		f += -GL_NEAREST_MIPMAP_NEAREST + GE_TEX_FILTER_NEAREST_MIPMAP_NEAREST;
	return (f & 0x7);
}


static
void update_clamp (void)
{
	int clamps = (pspgl_curctx->texture.wrap_s == GL_REPEAT) ? 0 : 1;
	int clampt = (pspgl_curctx->texture.wrap_t == GL_REPEAT) ? 0 : 1;
	sendCommandi(199, (clampt << 8) | clamps);
}


static
void update_texfilter (GLenum *dst, GLenum param)
{
	int min, mag;

	switch (param) {
	case GL_NEAREST:
	case GL_LINEAR:
	case GL_NEAREST_MIPMAP_NEAREST:
	case GL_LINEAR_MIPMAP_NEAREST:
	case GL_NEAREST_MIPMAP_LINEAR:
	case GL_LINEAR_MIPMAP_LINEAR:
		*dst = param;
		min = filter_gl2ge(pspgl_curctx->texture.min_filter);
		mag = filter_gl2ge(pspgl_curctx->texture.mag_filter);
		sendCommandi(198, (mag << 8) | min);
		break;
	default:
		GLERROR(GL_INVALID_ENUM);
	}
}


void glTexParameterfv (GLenum target, GLenum pname, const GLfloat *params)
{
	if (target != GL_TEXTURE_2D)
		goto invalid_enum;

	switch (pname) {
	case GL_TEXTURE_WRAP_S:
		pspgl_curctx->texture.wrap_s = params[0];
		update_clamp();
		break;
	case GL_TEXTURE_WRAP_T:
		pspgl_curctx->texture.wrap_t = params[0];
		update_clamp();
		break;
	case GL_TEXTURE_MAG_FILTER:
		update_texfilter(&pspgl_curctx->texture.mag_filter, params[0]);
		break;
	case GL_TEXTURE_MIN_FILTER:
		update_texfilter(&pspgl_curctx->texture.min_filter, params[0]);
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

