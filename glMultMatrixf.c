#include "pspgl_internal.h"


void glMultMatrixf (const GLfloat *m)
{
	int matrix_id = pspgl_curctx->matrix_mode;
	int depth = pspgl_curctx->matrix_stack_depth[matrix_id];
	GLfloat *matrix =  pspgl_curctx->matrix_stack[matrix_id][depth-1];
	int i;

	/**
	 *   Assumtion: (P != B). (P == A) is allowed.
	 *   Based on mesa code, initially contributed by Thomas Malik
	 */
	#define A(row,col)  matrix[4*col+row]
	#define B(row,col)  m[4*col+row]
	#define P(row,col)  matrix[4*col+row]

	for (i=0; i<4; i++) {
		const GLfloat ai0=A(i,0),  ai1=A(i,1),  ai2=A(i,2),  ai3=A(i,3);
		P(i,0) = ai0 * B(0,0) + ai1 * B(1,0) + ai2 * B(2,0) + ai3 * B(3,0);
		P(i,1) = ai0 * B(0,1) + ai1 * B(1,1) + ai2 * B(2,1) + ai3 * B(3,1);
		P(i,2) = ai0 * B(0,2) + ai1 * B(1,2) + ai2 * B(2,2) + ai3 * B(3,2);
		P(i,3) = ai0 * B(0,3) + ai1 * B(1,3) + ai2 * B(2,3) + ai3 * B(3,3);
	}

	pspgl_curctx->matrix_touched |= (1 << matrix_id);
}

