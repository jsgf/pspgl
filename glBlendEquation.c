#include "pspgl_internal.h"


void glBlendEquation (GLenum equation)
{
	switch (equation) {
	case GL_FUNC_ADD:
	case GL_FUNC_SUBTRACT:
	case GL_FUNC_REVERSE_SUBTRACT:
	case GL_MIN:
	case GL_MAX:
		pspgl_curctx->blend.equation = equation;
		glBlendFunc(pspgl_curctx->blend.sfactor,
			    pspgl_curctx->blend.dfactor);
		break;
	default:
		GLERROR(GL_INVALID_ENUM);
	}
}
