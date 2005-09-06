#include "pspgl_internal.h"


void glScalef (GLfloat x, GLfloat y, GLfloat z)
{
	int matrix_id = pspgl_curctx->matrix_mode;
	int depth = pspgl_curctx->matrix_stack_depth[matrix_id];
	GLfloat *m = pspgl_curctx->matrix_stack[matrix_id][depth-1];

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

	pspgl_curctx->matrix_touched |= (1 << matrix_id);
}

