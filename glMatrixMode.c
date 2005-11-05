#include "pspgl_internal.h"


void glMatrixMode (GLenum mode)
{
	struct pspgl_matrix_stack *s;

	switch(mode) {
	case GL_MODELVIEW:	s = &pspgl_curctx->modelview_stack; break;
	case GL_TEXTURE:	s = &pspgl_curctx->texture_stack; break;
	case GL_PROJECTION:	s = &pspgl_curctx->projection_stack; break;
	default:
		GLERROR(GL_INVALID_ENUM);
		return;
	}

	pspgl_curctx->current_matrix_stack = s;
	pspgl_curctx->current_matrix = &s->stack[s->depth];
}

