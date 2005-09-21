#include "pspgl_internal.h"


/* Blend Factor */
#define GE_SRC_COLOR		0
#define GE_ONE_MINUS_SRC_COLOR	1
#define GE_SRC_ALPHA		2
#define GE_ONE_MINUS_SRC_ALPHA	3
#define GE_DST_ALPHA		4
#define GE_ONE_MINUS_DST_ALPHA	5
#define GE_DST_COLOR		0
#define GE_ONE_MINUS_DST_COLOR	1
#define GE_FIX			10


void glBlendFunc (GLenum sfactor, GLenum dfactor)
{
	unsigned int srcfunc, dstfunc;

	switch (dfactor) {
	case GL_ZERO:
		sendCommandi(225, 0x000000);
		dstfunc = GE_FIX;
		break;
	case GL_ONE:
		sendCommandi(225, 0xffffff);
		dstfunc = GE_FIX;
		break;
	case GL_DST_COLOR:
		dstfunc = GE_DST_COLOR;
		break;
	case GL_ONE_MINUS_DST_COLOR:
		dstfunc = GE_ONE_MINUS_DST_COLOR;
		break;
	case GL_SRC_ALPHA:
		dstfunc = GE_SRC_ALPHA;
		break;
	case GL_ONE_MINUS_SRC_ALPHA:
		dstfunc = GE_ONE_MINUS_SRC_ALPHA;
		break;
	case GL_DST_ALPHA:
		dstfunc = GE_DST_ALPHA;
		break;
	case GL_ONE_MINUS_DST_ALPHA:
		dstfunc = GE_ONE_MINUS_DST_ALPHA;
		break;
	case GL_SRC_COLOR:
	case GL_ONE_MINUS_SRC_COLOR:
	default:
		GLERROR(GL_INVALID_ENUM);
		return;
	}

	switch (sfactor) {
	case GL_ZERO:
		sendCommandi(224, 0x000000);
		srcfunc = GE_FIX;
		break;
	case GL_ONE:
		sendCommandi(224, 0xffffff);
		srcfunc = GE_FIX;
		break;
	case GL_SRC_ALPHA:
		srcfunc = GE_SRC_ALPHA;
		break;
	case GL_ONE_MINUS_SRC_ALPHA:
		srcfunc = GE_ONE_MINUS_SRC_ALPHA;
		break;
	case GL_SRC_COLOR:
		srcfunc = GE_SRC_COLOR;
		break;
	case GL_ONE_MINUS_SRC_COLOR:
		srcfunc = GE_ONE_MINUS_SRC_COLOR;
		break;
	case GL_DST_ALPHA:
		srcfunc = GE_DST_ALPHA;
		break;
	case GL_ONE_MINUS_DST_ALPHA:
		srcfunc = GE_ONE_MINUS_DST_ALPHA;
		break;
	case GL_SRC_ALPHA_SATURATE:
	case GL_DST_COLOR:
	case GL_ONE_MINUS_DST_COLOR:
	default:
		GLERROR(GL_INVALID_ENUM);
		return;
	}

	pspgl_context_writereg_masked(pspgl_curctx, 223, (dstfunc << 4) | srcfunc, 0x0ff);
}

