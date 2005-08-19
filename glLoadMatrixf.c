#include "pspgl_internal.h"


void glLoadMatrixf (const GLfloat *m)
{
	int matrix_id = pspgl_curctx->matrix_mode & 0x03;
	int opcode = 60 + 2 * matrix_id;
	int n = (opcode == 62) ? 4 : 3;
	int i, j;

	for (i=0; i<16; i++)
		pspgl_curctx->matrix[matrix_id][i] = m[i];

	sendCommandi(opcode, pspgl_curctx->matrix_depth[matrix_id]);
	opcode++;

	for (j=0; j<4; j++) {
		for (i=0; i<n; i++) {
			sendCommandf(opcode, m[4*j+i]);
		}
	}
}

