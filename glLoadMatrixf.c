#include "pspgl_internal.h"


void glLoadMatrixf (const GLfloat *m)
{
	int matrix_id = pspgl_curctx->matrix_mode;
	int depth = pspgl_curctx->matrix_stack_depth[matrix_id];
	GLfloat *matrix = pspgl_curctx->matrix_stack[matrix_id][depth-1];
	int opcode = 60 + 2 * matrix_id;
	int n = (opcode == 62) ? 4 : 3;
	int i, j;

	for (i=0; i<16; i++)
		matrix[i] = m[i];

	sendCommandi(opcode, 0);
	opcode++;

	for (j=0; j<4; j++) {
		for (i=0; i<n; i++) {
			sendCommandf(opcode, m[4*j+i]);
		}
	}
}

