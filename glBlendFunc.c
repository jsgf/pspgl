#include "pspgl_internal.h"


/* Blend Equation */
#define GE_ADD               0
#define GE_SUBTRACT          1
#define GE_REVERSE_SUBTRACT  2
#define GE_MIN               3
#define GE_MAX               4
#define GE_ABS               5

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
	unsigned int srcfunc, dstfunc, equation;

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

	switch (pspgl_curctx->blend.equation) {
	case GL_FUNC_SUBTRACT:
		equation = GE_SUBTRACT;
		break;
	case GL_FUNC_REVERSE_SUBTRACT:
		equation = GE_REVERSE_SUBTRACT;
		break;
	case GL_MIN:
		equation = GE_MIN;
		break;
	case GL_MAX:
		equation = GE_MAX;
		break;
	case GL_FUNC_ADD:
	default:
		equation = GE_ADD;
	}

	pspgl_curctx->blend.sfactor = sfactor;
	pspgl_curctx->blend.dfactor = dfactor;

	sendCommandi(223, (equation << 8) | (srcfunc << 4) | dstfunc);
}
