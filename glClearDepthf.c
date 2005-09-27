#include "pspgl_internal.h"

void glClearDepthf (GLclampf depth)
{
	pspgl_curctx->clear.depth = 65535.0f * CLAMPF(depth);
}

