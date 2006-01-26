#include <pspvfpu.h>

#include "pspgl_internal.h"

const GLfloat __pspgl_identity[] = {
	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0,
};

void glLoadIdentity (void)
{
	struct pspgl_context *c = pspgl_curctx;
	struct pspgl_matrix_stack *stk = c->current_matrix_stack;
	struct pspgl_matrix *mat = c->current_matrix;

	assert(stk->flags & MF_VFPU);

	pspvfpu_use_matrices(c->vfpu_context, VFPU_STACKTOP, 0);

	mat->flags |= MF_IDENTITY;
	if (!(stk->flags & MF_DISABLED))
		stk->flags |= MF_DIRTY;

	asm volatile("vmidt.q m700");
}

