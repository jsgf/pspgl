#include "pspgl_internal.h"


void glLineWidth (GLfloat width)
{
	if (width != 1.0)
		GLERROR(GL_INVALID_ENUM);
}

