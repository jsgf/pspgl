#include "pspgl_internal.h"


void glLoadMatrixf (const GLfloat *m)
{
	int opcode, i, j, n;

	switch (pspgl_curctx->matrix_mode) {
#if 0
	case GL_MODELVIEW:
		n = 3;
		opcode = 58;  /* World matrix */
		break;
#endif
	case GL_MODELVIEW:
		opcode = 60;
		n = 3;
		break;
	case GL_PROJECTION:
		opcode = 62;
		n = 4;
		break;
	case GL_TEXTURE:
		opcode = 64;
		n = 3;
		break;
	default:
		GLERROR(GL_INVALID_ENUM);
		return;
	}

	sendCommandi(opcode, 0);
	opcode++;

	for (i=0; i<16; i++)
		pspgl_curctx->current_matrix[i] = m[i];

	for (j=0; j<4; j++) {
		for (i=0; i<n; i++) {
			sendCommandf(opcode, m[4*j+i]);
		}
	}
}
