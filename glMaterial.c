#include "pspgl_internal.h"


void glMaterialfv (GLenum face, GLenum pname, const GLfloat *params)
{
	switch (pname) {
	case GL_AMBIENT:
		sendCommandi(85, COLOR3(params));
		sendCommandi(88, (int) (255.0 * CLAMPF(params[3])));
		break;
	case GL_DIFFUSE:
		sendCommandi(84, COLOR3(params));
		sendCommandi(87, (int) (255.0 * CLAMPF(params[3])));
		break;
	case GL_AMBIENT_AND_DIFFUSE:
		glMaterialfv(face, GL_AMBIENT, params);
		glMaterialfv(face, GL_DIFFUSE, params);
		break;
	case GL_SPECULAR:
		sendCommandi(83, COLOR3(params));
		sendCommandi(86, (int) (255.0 * CLAMPF(params[3])));
		break;
	case GL_SHININESS:
		sendCommandf(91, params[0]);
		break;
	default:
		GLERROR(GL_INVALID_ENUM);
		return;
	}
}


void glMaterialf (GLenum face, GLenum pname, GLfloat param)
{
	glGetMaterialfv(face, pname, &param);
}

