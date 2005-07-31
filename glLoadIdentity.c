#include "pspgl_internal.h"


void glLoadIdentity (void)
{
	static const float identity [] = {
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0,
	};

	glLoadMatrixf(identity);
}

