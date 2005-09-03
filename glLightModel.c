#include "pspgl_internal.h"


void glLightModelfv (GLenum pname, const GLfloat *params)
{
	switch (pname) {
	case GL_LIGHT_MODEL_AMBIENT:
		sendCommandi(92, COLOR3(params));
		sendCommandi(93, (int) (255.0 * CLAMPF(params[3])));
		break;
	case GL_LIGHT_MODEL_COLOR_CONTROL:
		sendCommandi(94, (params[0] == GL_SEPARATE_SPECULAR_COLOR) ? 1 : 0);
		break;
	default:
		GLERROR(GL_INVALID_ENUM);
		return;
	}
}


void glLightModelf (GLenum pname, GLfloat param)
{
	glLightModelfv(pname, &param);
}

