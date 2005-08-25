#include "pspgl_internal.h"

void glClearDepth (GLclampd depth)
{
	pspgl_curctx->clear.depth = CLAMPF((GLfloat) depth);
}

