#include "pspgl_internal.h"


void glLoadMatrixf (const GLfloat *m)
{
	GLfloat *matrix = pspgl_curctx->current_matrix->mat;
	int i;

	for (i=0; i<16; i++)
		matrix[i] = m[i];

	if (!(pspgl_curctx->current_matrix_stack->flags & MF_DISABLED))
		pspgl_curctx->current_matrix_stack->flags |= MF_DIRTY;
}
