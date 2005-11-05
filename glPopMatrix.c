#include "pspgl_internal.h"
#include <stdlib.h>


void glPopMatrix (void)
{
	struct pspgl_context *c = pspgl_curctx;
	struct pspgl_matrix_stack *curstk = c->current_matrix_stack;

	if (curstk->depth == 0) {
		GLERROR(GL_STACK_UNDERFLOW);
		return;
	}

	c->current_matrix = &curstk->stack[--curstk->depth];
	curstk->dirty = 1;
}

