#include "pspgl_internal.h"


static
int map_sfunc (GLenum opcode)
{
	int ret;

	switch (opcode) {
	case GL_ZERO:
		ret = 1;
		break;
	case GL_INVERT:
		ret = 3;
		break;
	case GL_KEEP:
		ret = 0;
		break;
	case GL_REPLACE:
		ret = 2;
		break;
	case GL_INCR:
		ret = 4;
		break;
	case GL_DECR:
		ret = 5;
		break;
	default:
		GLERROR(GL_INVALID_ENUM);
		ret = 0;
	}
	
	return ret;
}

void glStencilOp (GLenum fail, GLenum zfail, GLenum zpass)
{
	sendCommandi(221, (map_sfunc(zpass) << 16) | (map_sfunc(zfail) << 8) | map_sfunc(fail));
}
