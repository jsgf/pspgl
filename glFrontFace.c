#include "pspgl_internal.h"


void glFrontFace (GLenum mode)
{
	switch (mode) {
	case GL_CW:
	case GL_CCW:
		sendCommandi(155, (~mode & 1));
		break;
	default:
		GLERROR(GL_INVALID_ENUM);
	}
}

