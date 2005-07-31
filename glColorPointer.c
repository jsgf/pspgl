#include "pspgl_internal.h"


void glColorPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
	pspgl_curctx->vertex_array.color.size = size;
	pspgl_curctx->vertex_array.color.type = type;
	pspgl_curctx->vertex_array.color.stride = stride;
	pspgl_curctx->vertex_array.color.ptr = pointer;
}
