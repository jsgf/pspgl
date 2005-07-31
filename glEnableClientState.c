#include "pspgl_internal.h"


void pspgl_enable_clientstate (GLenum array, GLenum enable)
{
	switch (array) {
	case GL_VERTEX_ARRAY:
		pspgl_curctx->vertex_array.vertex.enabled = enable;
		break;
	case GL_COLOR_ARRAY:
		pspgl_curctx->vertex_array.color.enabled = enable;
		break;
	case GL_NORMAL_ARRAY:
		pspgl_curctx->vertex_array.normal.enabled = enable;
		break;
	case GL_TEXTURE_COORD_ARRAY:
		pspgl_curctx->vertex_array.texcoord.enabled = enable;
		break;
	default:
		GLERROR(GL_INVALID_ENUM);
	}
}


void glEnableClientState (GLenum array)
{
	pspgl_enable_clientstate(array, 1);
}


void glDisableClientState (GLenum array)
{
	pspgl_enable_clientstate(array, 0);
}
