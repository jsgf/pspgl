#include <string.h>

#include "pspgl_internal.h"
#include "pspgl_buffers.h"

void glGetBufferSubDataARB(GLenum target, GLintptrARB offset,
			   GLsizeiptrARB size, GLvoid *data)
{
	struct pspgl_bufferobj *buf, **bufp;
	void *p;

	bufp = __pspgl_bufferobj_for_target(target);

	if (bufp == NULL)
		return;

	buf = *bufp;

	if (buf == NULL || buf->data == NULL || buf->mapped) {
		GLERROR(GL_INVALID_OPERATION);
		return;
	}

	if (size < 0 || offset+size > buf->data->size) {
		GLERROR(GL_INVALID_VALUE);
		return;
	}

	p = __pspgl_buffer_map(buf->data, GL_READ_ONLY);

	memcpy(data, p+offset, size);

	__pspgl_buffer_unmap(buf->data, GL_READ_ONLY);
}

void glGetBufferSubData(GLenum target, GLintptrARB offset,
			GLsizeiptrARB size, GLvoid *data)
	__attribute__((alias("glGetBufferSubDataARB")));
