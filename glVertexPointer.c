#include "pspgl_internal.h"


void glVertexPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
	if (size < 2 || size > 4) {
		GLERROR(GL_INVALID_VALUE);
		return;
	}

	if (type != GL_BYTE && type != GL_SHORT && type != GL_FLOAT) {
		GLERROR(GL_INVALID_ENUM);
		return;
	}

	if (stride < 0) {
		GLERROR(GL_INVALID_VALUE);
		return;
	}

	if (stride == 0)
		stride = __pspgl_gl_sizeof(type) * size;

	pspgl_curctx->vertex_array.vertex.size = size;
	pspgl_curctx->vertex_array.vertex.type = type;
	pspgl_curctx->vertex_array.vertex.stride = stride;
	pspgl_curctx->vertex_array.vertex.ptr = pointer;
}
