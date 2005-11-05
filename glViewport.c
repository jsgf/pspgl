#include "pspgl_internal.h"


void glViewport (GLint x, GLint y, GLsizei width, GLsizei height)
{
	pspgl_curctx->viewport.x = x;
	pspgl_curctx->viewport.y = y;
	pspgl_curctx->viewport.width = width;
	pspgl_curctx->viewport.height = height;

	if (x < 0 || y < 0 || width < 0 || height < 0 ||
	    x+width > pspgl_curctx->draw->width ||
	    y+height > pspgl_curctx->draw->height)
	{
		GLERROR(GL_INVALID_VALUE);
		return;
	}

	/* Viewport / Screen Offset */
	sendCommandi(CMD_OFFSETX, (2048 - width/2) << 4);
	sendCommandi(CMD_OFFSETY, (2048 - height/2) << 4);

	/* Viewport Size (X/Y, Width/Height) */
	sendCommandf(CMD_VIEWPORT_SX, (float) (width/2));
	sendCommandf(CMD_VIEWPORT_SY, (float) (-height/2));

	/* Viewport Center (X/Y) */
	sendCommandf(CMD_VIEWPORT_TX, (float) (2048));
	sendCommandf(CMD_VIEWPORT_TY, (float) (2048));

	/* Drawing Rectangle */
	sendCommandi(CMD_REGION1, (y << 10) | x);
	sendCommandi(CMD_REGION2, (((y + height)-1) << 10) | ((x + width)-1));
}
