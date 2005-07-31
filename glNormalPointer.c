#include "pspgl_internal.h"


void glNormalPointer (GLenum type, GLsizei stride, const GLvoid *pointer)
{
	pspgl_curctx->vertex_array.normal.type = type;
	pspgl_curctx->vertex_array.normal.stride = stride;
	pspgl_curctx->vertex_array.normal.ptr = pointer;
}
