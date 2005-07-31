#include "pspgl_internal.h"


void glCullFace (GLenum mode)
{
	switch (mode) {
	case GL_BACK:
		break;
	case GL_FRONT:
	case GL_FRONT_AND_BACK:
	default:
		GLERROR(GL_INVALID_ENUM);
	}
}

