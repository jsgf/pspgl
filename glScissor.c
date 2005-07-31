#include "pspgl_internal.h"


void glScissor (GLint x, GLint y, GLsizei width, GLsizei height)
{
	pspgl_curctx->scissor_test.x = x;
	pspgl_curctx->scissor_test.y = y;
	pspgl_curctx->scissor_test.width = width;
	pspgl_curctx->scissor_test.height = height;

	sendCommandi(212, (y << 10) | x);
	sendCommandi(213, (((y + height) - 1) << 10) | (((x + width) - 1)));
}

