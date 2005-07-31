#include "pspgl_internal.h"


void glScalef (GLfloat x, GLfloat y, GLfloat z)
{
	GLfloat *m = pspgl_curctx->current_matrix;

	m[0] *= x;
	m[1] *= x;
	m[2] *= x;
	m[3] *= x;

	m[4] *= y;
	m[5] *= y;
	m[6] *= y;
	m[7] *= y;

	m[8] *= z;
	m[9] *= z;
	m[10] *= z;
	m[11] *= z;

	glLoadMatrixf(m);
}
