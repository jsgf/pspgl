#include "pspgl_internal.h"


void glGetFloatv (GLenum pname, GLfloat *params)
{
	struct pspgl_matrix_stack *s;

	switch (pname) {
	case GL_LINE_WIDTH:
		params[0] = 1.0f;
		break;
	case GL_ALIASED_POINT_SIZE_RANGE:
	case GL_SMOOTH_POINT_SIZE_RANGE:
	case GL_SMOOTH_POINT_SIZE_GRANULARITY:
	case GL_ALIASED_LINE_WIDTH_RANGE:
	case GL_SMOOTH_LINE_WIDTH_RANGE:
	case GL_SMOOTH_LINE_WIDTH_GRANULARITY:
		params[0] = 1.0f;
		params[1] = 1.0f;
		break;
	case GL_COLOR_CLEAR_VALUE:
		params[0] = ((pspgl_curctx->clear.color >> 0) & 0xff) / 255.f;
		params[1] = ((pspgl_curctx->clear.color >> 8) & 0xff) / 255.f;
		params[2] = ((pspgl_curctx->clear.color >>16) & 0xff) / 255.f;
		params[3] = ((pspgl_curctx->clear.color >>24) & 0xff) / 255.f;
		break;
	case GL_MODELVIEW_MATRIX:	s = &pspgl_curctx->modelview_stack; goto get_matrix;
	case GL_TEXTURE_MATRIX:		s = &pspgl_curctx->texture_stack; goto get_matrix;
	case GL_PROJECTION_MATRIX:	s = &pspgl_curctx->projection_stack; goto get_matrix;
	get_matrix:
		if (params) {
			GLfloat *matrix = s->stack[s->depth].mat;
			int i;

			for (i=0; i<16; i++)
				params[i] = matrix[i];
		}
		break;
	default:
		GLERROR(GL_INVALID_ENUM);
	}
}

