#include "pspgl_internal.h"


void glFrontFace (GLenum mode)
{
	switch (mode) {
	case GL_CW:
	case GL_CCW:
		pspgl_curctx->polygon.front_cw = (mode == GL_CW);

		/* Because the PSP uses a right-handed screen coord system,
		   the cull face direction is reversed with respect to the
		   normal sense of CMD_CULL_FACE. */
		sendCommandi(CMD_CULL_FACE, (pspgl_curctx->polygon.front_cw ^ pspgl_curctx->polygon.cull_front));
		break;
	default:
		GLERROR(GL_INVALID_ENUM);
	}
}

