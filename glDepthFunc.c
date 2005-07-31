#include "pspgl_internal.h"


static const char depthfunc_mapping [] = { 0, 4, 2, 5, 6, 3, 7, 1 };

void glDepthFunc (GLenum func)
{
	if (func < GL_NEVER || func > GL_ALWAYS) {
		GLERROR(GL_INVALID_ENUM);
		return;
	}

	sendCommandi(222, depthfunc_mapping[func - GL_NEVER]);
}
