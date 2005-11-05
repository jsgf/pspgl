#include "pspgl_internal.h"


void glTexCoordPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
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

	native = (size == 2);

	psp_log("ptr=%p size=%d type=%x stride=%d native=%d\n",
		pointer, size, type, stride, native);

	pspgl_curctx->vertex_array.texcoord.size = size;
	pspgl_curctx->vertex_array.texcoord.type = type;
	pspgl_curctx->vertex_array.texcoord.stride = stride;
	pspgl_curctx->vertex_array.texcoord.ptr = pointer;
	pspgl_curctx->vertex_array.texcoord.native = native;
}
