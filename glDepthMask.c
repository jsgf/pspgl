#include "pspgl_internal.h"


void glDepthMask (GLboolean flag)
{
	sendCommandi(231, flag ? 0 : ~0);
}

