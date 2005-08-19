#include "pspgl_internal.h"


void glTranslatef (GLfloat x, GLfloat y, GLfloat z)
{
	int matrix_id = pspgl_curctx->matrix_mode & 0x03;
	GLfloat *m = pspgl_curctx->matrix[matrix_id];

	m[12] += x * m[0] + y * m[4] + z * m[8];
	m[13] += x * m[1] + y * m[5] + z * m[9];
	m[14] += x * m[2] + y * m[6] + z * m[10];
	m[15] += x * m[3] + y * m[7] + z * m[11];

	glLoadMatrixf(m);
}
