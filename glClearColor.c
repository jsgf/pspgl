#include "pspgl_internal.h"

void glClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
	pspgl_curctx->clear.color[0] = CLAMPF(red);
	pspgl_curctx->clear.color[1] = CLAMPF(green);
	pspgl_curctx->clear.color[2] = CLAMPF(blue);
	pspgl_curctx->clear.color[3] = CLAMPF(alpha);
}

