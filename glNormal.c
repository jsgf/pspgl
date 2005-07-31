#include "pspgl_internal.h"


void glNormal3f (GLfloat nx, GLfloat ny, GLfloat nz)
{
	pspgl_curctx->current.normal[0] = nx;
	pspgl_curctx->current.normal[1] = ny;
	pspgl_curctx->current.normal[2] = nz;
}
