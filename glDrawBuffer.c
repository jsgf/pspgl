#include "pspgl_internal.h"


void glDrawBuffer (GLenum mode)
{
	switch (mode) {
	case GL_BACK:
		break;
	default:
		/* XXX IMPROVE: support front & aux buffers */
		GLERROR(GL_INVALID_ENUM);
	}
}

