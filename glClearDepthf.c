#include "pspgl_internal.h"

void glClearDepthf (GLclampf depth)
{
	pspgl_curctx->clear.depth = CLAMPF(depth);
}

