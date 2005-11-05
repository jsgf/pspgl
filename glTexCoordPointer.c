#include "pspgl_internal.h"


void glTexCoordPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
	struct pspgl_vertex_array *va = &pspgl_curctx->vertex_array.texcoord;
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

	va->size = size;
	va->type = type;
	va->stride = stride;
	va->ptr = pointer;
	va->native = native;

	__pspgl_varray_bind_buffer(va, pspgl_curctx->vertex_array.arraybuffer);
}
