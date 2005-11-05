#include "pspgl_internal.h"


const GLfloat __pspgl_identity [] = {
	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0,
};

void glLoadIdentity (void)
{
	glLoadMatrixf(__pspgl_identity);
}

