#include "pspgl_internal.h"


void glScissor (GLint x, GLint y, GLsizei width, GLsizei height)
{
	int t,b,l,r;

	pspgl_curctx->scissor_test.x = x;
	pspgl_curctx->scissor_test.y = y;
	pspgl_curctx->scissor_test.width = width;
	pspgl_curctx->scissor_test.height = height;

	b = pspgl_curctx->draw->height - y;
	t = b - height;
	l = x;
	r = x + width - 1;

	sendCommandi(CMD_SCISSOR1, (t << 10) | l);
	sendCommandi(CMD_SCISSOR2, ((b-1) << 10) | r);
}

