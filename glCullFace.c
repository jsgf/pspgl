#include "pspgl_internal.h"


void glCullFace (GLenum mode)
{
	switch (mode) {
	case GL_BACK:
	case GL_FRONT:
		pspgl_curctx->cull_front = (mode == GL_FRONT);

		/* Because the PSP uses a right-handed screen coord system,
		   the cull face direction is reversed with respect to the
		   normal sense of CMD_CULL_FACE. */
		sendCommandi(CMD_CULL_FACE, (pspgl_curctx->front_cw ^ pspgl_curctx->cull_front));
		break;

	case GL_FRONT_AND_BACK:
	default:
		GLERROR(GL_INVALID_ENUM);
	}
}

