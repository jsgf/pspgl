#include "pspgl_internal.h"


void glColorMask (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
	unsigned long mask = red ? 0xffff00 : 0xffffff;

	if (green)
		mask &= 0xff00ff;

	if (blue)
		mask &= 0x00ffff;

	pspgl_curctx->write_mask.alpha = alpha ? 0x00: 0xff;

        sendCommandi(232, mask);

	/**
	 * Alpha Channel and Stencil are shared. Only update Alpha mask register
	 * if stencil test is disabled.
	 */
	if ((pspgl_curctx->ge_reg[36] & 1) == 0)
	        sendCommandi(233, pspgl_curctx->write_mask.alpha);
}

