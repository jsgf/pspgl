#include "pspgl_internal.h"


void glTexCoordPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
	pspgl_curctx->vertex_array.texcoord.size = size;
	pspgl_curctx->vertex_array.texcoord.type = type;
	pspgl_curctx->vertex_array.texcoord.stride = stride;
	pspgl_curctx->vertex_array.texcoord.ptr = pointer;
}
