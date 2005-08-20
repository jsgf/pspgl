#include "pspgl_internal.h"


void glPushMatrix (void)
{
	struct pspgl_context *c = pspgl_curctx;
	int matrix_id = c->matrix_mode & 0x03;

	/* XXX TODO: check overflow! */
	c->matrix_depth[matrix_id]++;
	glLoadMatrixf(c->matrix[matrix_id]);
}

