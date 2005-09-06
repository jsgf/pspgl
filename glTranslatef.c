#include "pspgl_internal.h"


void glTranslatef (GLfloat x, GLfloat y, GLfloat z)
{
	int matrix_id = pspgl_curctx->matrix_mode;
	int depth = pspgl_curctx->matrix_stack_depth[matrix_id];
	GLfloat *m = pspgl_curctx->matrix_stack[matrix_id][depth-1];

	m[12] += x * m[0] + y * m[4] + z * m[8];
	m[13] += x * m[1] + y * m[5] + z * m[9];
	m[14] += x * m[2] + y * m[6] + z * m[10];
	m[15] += x * m[3] + y * m[7] + z * m[11];

	pspgl_curctx->matrix_touched |= (1 << matrix_id);
}

