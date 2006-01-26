#include "pspgl_internal.h"


void glTranslatef (GLfloat x, GLfloat y, GLfloat z)
{
	GLfloat *m = pspgl_curctx->current_matrix->mat;

	m[12] += x * m[0] + y * m[4] + z * m[8];
	m[13] += x * m[1] + y * m[5] + z * m[9];
	m[14] += x * m[2] + y * m[6] + z * m[10];
	m[15] += x * m[3] + y * m[7] + z * m[11];

	if (!(pspgl_curctx->current_matrix_stack->flags & MF_DISABLED))
		pspgl_curctx->current_matrix_stack->flags |= MF_DIRTY;

	pspgl_curctx->current_matrix->flags &= ~MF_IDENTITY;
}

