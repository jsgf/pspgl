#include "pspgl_internal.h"


void glLoadMatrixf (const GLfloat *m)
{
	int matrix_id = pspgl_curctx->matrix_mode;
	int depth = pspgl_curctx->matrix_stack_depth[matrix_id];
	GLfloat *matrix = pspgl_curctx->matrix_stack[matrix_id][depth-1];
	int i;

	for (i=0; i<16; i++)
		matrix[i] = m[i];

	pspgl_curctx->matrix_touched |= (1 << matrix_id);
}

