#include "pspgl_internal.h"


void glMatrixMode (GLenum mode)
{
	switch (mode) {
	case GL_MODELVIEW:
		pspgl_curctx->current_matrix = pspgl_curctx->modelview_matrix;
		pspgl_curctx->matrix_mode = mode;
		break;
	case GL_PROJECTION:
		pspgl_curctx->current_matrix = pspgl_curctx->projection_matrix;
		pspgl_curctx->matrix_mode = mode;
		break;
	case GL_TEXTURE:
		pspgl_curctx->current_matrix = pspgl_curctx->texture_matrix;
		pspgl_curctx->matrix_mode = mode;
		break;
	default:
		GLERROR(GL_INVALID_ENUM);
	}
}

