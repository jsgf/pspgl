#include <string.h>

#include "pspgl_internal.h"
#include "pspgl_buffers.h"

void glBufferSubDataARB(GLenum target, GLintptrARB offset, GLsizeiptrARB size,
			const GLvoid *data)
{
	struct pspgl_bufferobj *buf, **bufp;
	void *p;

	bufp = __pspgl_bufferobj_for_target(target);

	if (bufp == NULL)
		return;

	buf = *bufp;

	if (buf == NULL || buf->mapped) {
		GLERROR(GL_INVALID_OPERATION);
		return;
	}

	if (size < 0 || 
	    buf->data == NULL || 
	    size+offset > buf->data->size) {
		GLERROR(GL_INVALID_VALUE);
		return;
	}

	__pspgl_buffer_dlist_sync(buf->data);

	p = __pspgl_buffer_map(buf->data, GL_WRITE_ONLY_ARB);

	memcpy(p + offset, data, size);

	__pspgl_buffer_unmap(buf->data, GL_WRITE_ONLY_ARB);
}

void glBufferSubData(GLenum target, GLintptrARB offset, GLsizeiptrARB size,
		     const GLvoid *data)
	__attribute__((alias("glBufferSubDataARB")));

