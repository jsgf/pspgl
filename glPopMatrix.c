#include "pspgl_internal.h"
#include <stdlib.h>


void glPopMatrix (void)
{
	struct pspgl_context *c = pspgl_curctx;
	int matrix_id = c->matrix_mode;
	int depth = c->matrix_stack_depth[matrix_id];

	if (depth <= 1) {
		GLERROR(GL_STACK_UNDERFLOW);
		return;
	}

	c->matrix_stack[matrix_id] = realloc(c->matrix_stack[matrix_id], (depth - 1) * sizeof(c->matrix_stack[0][0]));
	c->matrix_stack_depth[matrix_id]--;

	pspgl_curctx->matrix_touched |= (1 << matrix_id);
}

