#include "pspgl_internal.h"
#include <stdlib.h>
#include <string.h>


void glPushMatrix (void)
{
	struct pspgl_context *c = pspgl_curctx;
	int matrix_id = c->matrix_mode;
	int depth = c->matrix_stack_depth[matrix_id];
	void *tmp;

	tmp = realloc(c->matrix_stack[matrix_id], (depth + 1) * sizeof(c->matrix_stack[0][0]));

	if (!tmp) {
		GLERROR(GL_OUT_OF_MEMORY);
		return;
	}

	c->matrix_stack[matrix_id] = tmp;
	c->matrix_stack_depth[matrix_id]++;

	if (depth > 0)
		memcpy(c->matrix_stack[matrix_id][depth], c->matrix_stack[matrix_id][depth-1], sizeof(c->matrix_stack[0][0]));

	pspgl_curctx->matrix_touched |= (1 << matrix_id);
}

