#include "pspgl_internal.h"


void glMatrixMode (GLenum mode)
{
	if (mode < GL_MODELVIEW || mode > GL_TEXTURE) {
		GLERROR(GL_INVALID_ENUM);
	} else {
		pspgl_curctx->matrix_mode = mode - GL_MODELVIEW;
	}
}

