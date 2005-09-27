#include "pspgl_internal.h"

void glClearDepth (GLclampd depth)
{
	pspgl_curctx->clear.depth = 65535.0f * CLAMPF((GLfloat) depth);
}

