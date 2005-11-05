#include "pspgl_internal.h"


void glMatrixMode (GLenum mode)
{
	struct pspgl_matrix_stack *s;
	struct pspgl_context *c = pspgl_curctx;

	switch(mode) {
	case GL_MODELVIEW:	s = &c->modelview_stack; break;
	case GL_TEXTURE:	s = &c->texture_stack; break;
	case GL_PROJECTION:	s = &c->projection_stack; break;
	case GL_VIEW_PSP:	s = &c->view_stack; break;

	case GL_BONE0_PSP ... GL_BONE7_PSP:
		s = &c->bone_stacks[mode - GL_BONE0_PSP];
		break;

	default:
		GLERROR(GL_INVALID_ENUM);
		return;
	}

	c->current_matrix_stack = s;
	c->current_matrix = &s->stack[s->depth];
}

