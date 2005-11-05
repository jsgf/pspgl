#include "pspgl_internal.h"


void glNormalPointer (GLenum type, GLsizei stride, const GLvoid *pointer)
{
	if (type != GL_BYTE && type != GL_SHORT && type != GL_FLOAT) {
		GLERROR(GL_INVALID_ENUM);
		return;
	}

	if (stride < 0) {
		GLERROR(GL_INVALID_VALUE);
		return;
	}

	if (stride == 0)
		stride = __pspgl_gl_sizeof(type) * 3;

	pspgl_curctx->vertex_array.normal.size = 3;
	pspgl_curctx->vertex_array.normal.type = type;
	pspgl_curctx->vertex_array.normal.stride = stride;
	pspgl_curctx->vertex_array.normal.ptr = pointer;
}
