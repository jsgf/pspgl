#include "pspgl_internal.h"
#include <pspge.h>

void glPopMatrix (void)
{
	struct pspgl_context *c = pspgl_curctx;
	int matrix_id = c->matrix_mode & 0x03;

	if (c->matrix_depth[matrix_id] == 0) {
		GLERROR(GL_STACK_UNDERFLOW);
		return;
	}

	c->matrix_depth[matrix_id]--;

	/* need to wait until pending matrix transforms are finished... */
	sendCommandi(60 + 2 * matrix_id, pspgl_curctx->matrix_depth[matrix_id]);
	glFlush();

	sceGeGetMtx(c->matrix_depth[matrix_id], c->matrix[matrix_id]);
}

