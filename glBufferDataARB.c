#include <string.h>
#include <stdlib.h>
#include <malloc.h>

#include "pspgl_internal.h"
#include "pspgl_buffers.h"

void glBufferDataARB(GLenum target, GLsizeiptr size,
		     const GLvoid *init_data, GLenum usage)
{
	struct pspgl_bufferobj *buf, **bufp;
	struct pspgl_buffer *databuf;

	bufp = __pspgl_bufferobj_for_target(target);

	if (bufp == NULL)
		return;

	buf = *bufp;

	if (buf == NULL) {
		GLERROR(GL_INVALID_OPERATION);
		return;
	}

	if (size < 0) {
		GLERROR(GL_INVALID_VALUE);
		return;
	}

	databuf = __pspgl_buffer_new(size, usage);

	if (databuf == NULL) {
		GLERROR(GL_OUT_OF_MEMORY);
		return;
	}

	if (buf->data) {
		if (buf->mapped)
			__pspgl_buffer_unmap(buf->data, buf->access);

		psp_log("freeing data %p for buffer %p\n", buf->data, buf);
		__pspgl_buffer_free(buf->data);
	}

	buf->mapped = GL_FALSE;
	buf->usage = usage;
	buf->data = databuf;

	psp_log("attaching data %p(%p) to buffer %p\n",
		databuf, databuf->base, buf);

	if (init_data != NULL) {
		void *p;

		p = __pspgl_buffer_map(databuf, GL_WRITE_ONLY_ARB);

		memcpy(p, init_data, size);

		__pspgl_buffer_unmap(databuf, GL_WRITE_ONLY_ARB);
	}
}

void glBufferData(GLenum target, GLsizeiptr size,
		  const GLvoid *init_data, GLenum usage)
	__attribute__((alias("glBufferDataARB")));
