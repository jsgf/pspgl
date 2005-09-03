#include "pspgl_internal.h"


void glStencilMask (GLuint mask)
{
	pspgl_curctx->write_mask.stencil = mask;

	/**
	 * Alpha Channel and Stencil are shared. Only update mask register
	 * if stencil test is enabled.
	 */
	if (pspgl_curctx->ge_reg[36] & 1)
	        sendCommandi(233, pspgl_curctx->write_mask.alpha);
}

