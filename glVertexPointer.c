#include "pspgl_internal.h"


void glVertexPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
	pspgl_curctx->vertex_array.vertex.size = size;
	pspgl_curctx->vertex_array.vertex.type = type;
	pspgl_curctx->vertex_array.vertex.stride = stride;
	pspgl_curctx->vertex_array.vertex.ptr = pointer;
}
