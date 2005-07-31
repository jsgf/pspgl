#include "pspgl_internal.h"


#define GE_TEXENV_MODULATE				0
#define GE_TEXENV_DECAL					1
#define GE_TEXENV_BLEND					2
#define GE_TEXENV_REPLACE				3
#define GE_TEXENV_ADD					4

#define GE_TEXENV_RGB					0
#define GE_TEXENV_RGBA					1


void glTexEnvfv (GLenum target, GLenum pname, const GLfloat *params)
{
	int mode;

	if (target != GL_TEXTURE_ENV) {
		GLERROR(GL_INVALID_ENUM);
		return;
	}

	switch (pname) {
	case GL_TEXTURE_ENV_MODE:
		switch ((GLenum) params[0]) {
		case GL_MODULATE:
			mode = GE_TEXENV_MODULATE;
			break;
		case GL_DECAL:
			mode = GE_TEXENV_DECAL;
			break;
		case GL_BLEND:
			mode = GE_TEXENV_BLEND;
			break;
		case GL_REPLACE:
			mode = GE_TEXENV_REPLACE;
			break;
		case GL_ADD:
			mode = GE_TEXENV_ADD;
			break;
		default:
			goto invalid_enum;
		}
		/* XXX checkme: RGBA or RGB? should it depend on texture format? */
		sendCommandi(201, (GE_TEXENV_RGBA << 16) | (GE_TEXENV_RGBA << 8) | mode);
		break;
	case GL_TEXTURE_ENV_COLOR:
		sendCommandi(202, COLOR(params));
		break;
	default:
		goto invalid_enum;
	}

	return;
	
invalid_enum:
	GLERROR(GL_INVALID_ENUM);
}


void glTexEnvf (GLenum target, GLenum pname, GLfloat param)
{
	glTexEnvfv(target, pname, &param);
}


void glTexEnvi (GLenum target, GLenum pname, GLint param)
{
	GLfloat p = param;
	glTexEnvfv(target, pname, &p);
}
