#include "pspgl_internal.h"


void glVertexPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
	GLboolean native;

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

	native = (size == 3);

	psp_log("ptr=%p size=%d type=%x stride=%d native=%d\n",
		pointer, size, type, stride, native);

	pspgl_curctx->vertex_array.vertex.size = size;
	pspgl_curctx->vertex_array.vertex.type = type;
	pspgl_curctx->vertex_array.vertex.stride = stride;
	pspgl_curctx->vertex_array.vertex.ptr = pointer;
	pspgl_curctx->vertex_array.vertex.native = native;
}
