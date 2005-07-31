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
	sendCommandi(76, (2048 - width/2) << 4);
	sendCommandi(77, (2048 - height/2) << 4);

	/* Viewport Size (X/Y, Width/Height) */
	sendCommandf(66, (float) (width/2));
	sendCommandf(67, (float) (-height/2));

	/* Viewport Center (X/Y) */
	sendCommandf(69, (float) (2048));
	sendCommandf(70, (float) (2048));

	/* Drawing Rectangle */
	sendCommandi(21, (y << 10) | x);
	sendCommandi(22, (((y + height)-1) << 10) | ((x + width)-1));
}
