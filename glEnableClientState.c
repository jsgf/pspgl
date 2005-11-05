#include "pspgl_internal.h"


static void enable_clientstate (GLenum array, GLboolean enable)
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
	enable_clientstate(array, GL_TRUE);
}


void glDisableClientState (GLenum array)
{
	enable_clientstate(array, GL_FALSE);
}
