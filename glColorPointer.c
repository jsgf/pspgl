#include "pspgl_internal.h"


void glColorPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
	GLboolean native;

	if (size != 3 && size != 4) {
		GLERROR(GL_INVALID_VALUE);
		return;
	}

	if (type != GL_FLOAT && type != GL_UNSIGNED_BYTE) {
		GLERROR(GL_INVALID_ENUM);
		return;
	}

	if (stride < 0) {
		GLERROR(GL_INVALID_VALUE);
		return;
	}

	if (stride == 0)
		stride = __pspgl_gl_sizeof(type) * size;

	native = (type == GL_UNSIGNED_BYTE) && (size == 4);

	psp_log("ptr=%p size=%d type=%x stride=%d native=%d\n",
		pointer, size, type, stride, native);

	pspgl_curctx->vertex_array.color.size = size;
	pspgl_curctx->vertex_array.color.type = type;
	pspgl_curctx->vertex_array.color.stride = stride;
	pspgl_curctx->vertex_array.color.ptr = pointer;
}
