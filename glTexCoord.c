#include "pspgl_internal.h"


void glTexCoord2f (GLfloat s, GLfloat t)
{
	pspgl_curctx->current.texcoord[0] = s;
	pspgl_curctx->current.texcoord[1] = t;
}
