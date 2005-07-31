#include "pspgl_internal.h"


void glBegin (GLenum mode)
{
	if (mode >= GL_POINTS && mode <= GL_POLYGON) {
		pspgl_curctx->current.primitive = mode;
		pspgl_curctx->current.vertex_count = 0;
	} else {
		GLERROR(GL_INVALID_ENUM);
	}
}
