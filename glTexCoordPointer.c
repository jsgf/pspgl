#include "pspgl_internal.h"


void glTexCoordPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
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

	pspgl_curctx->vertex_array.texcoord.size = size;
	pspgl_curctx->vertex_array.texcoord.type = type;
	pspgl_curctx->vertex_array.texcoord.stride = stride;
	pspgl_curctx->vertex_array.texcoord.ptr = pointer;
}
