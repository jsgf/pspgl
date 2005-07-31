#include "pspgl_internal.h"


static const char stenciltestfunc_mapping [] = { 0, 4, 2, 5, 6, 3, 7, 1 };

void glStencilFunc( GLenum func, GLint ref, GLuint mask)
{
	unsigned char sref = (unsigned char) ref;

	if (func < GL_NEVER || func > GL_ALWAYS) {
		GLERROR(GL_INVALID_ENUM);
		return;
	}

	func &= 0x0007;

	sendCommandi(219, (mask << 16) | (sref << 8) | stenciltestfunc_mapping[func]);
}
