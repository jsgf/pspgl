#include "pspgl_internal.h"


static const char alphatestfunc_mapping [] = { 0, 4, 2, 5, 6, 3, 7, 1 };

void glAlphaFunc (GLenum func, GLclampf ref)
{
	unsigned char aref = (unsigned char) (255.0f * CLAMPF(ref));
	unsigned char amsk = 0xff;

	if (func < GL_NEVER || func > GL_ALWAYS) {
		GLERROR(GL_INVALID_ENUM);
		return;
	}

	func &= 0x0007;

	sendCommandi(219, (amsk << 16) | (aref << 8) | alphatestfunc_mapping[func]);
}
