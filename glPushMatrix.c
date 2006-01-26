#include "pspgl_internal.h"
#include <stdlib.h>
#include <string.h>


void glPushMatrix (void)
{
	struct pspgl_context *c = pspgl_curctx;
	struct pspgl_matrix_stack *curstk = c->current_matrix_stack;

	if (unlikely(++curstk->depth == curstk->limit))
		goto out_error;

	c->current_matrix = &curstk->stack[curstk->depth];

	memcpy(c->current_matrix, c->current_matrix-1, sizeof(struct pspgl_matrix));
	return;

  out_error:
	curstk->depth--;
	GLERROR(GL_STACK_OVERFLOW);
}

