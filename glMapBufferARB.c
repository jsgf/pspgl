#include "pspgl_internal.h"
#include "pspgl_buffers.h"

GLvoid *glMapBufferARB(GLenum target, GLenum access)
{
	struct pspgl_bufferobj *buf, **bufp;
	void *p;

	bufp = __pspgl_bufferobj_for_target(target);

	if (bufp == NULL)
		return NULL;

	buf = *bufp;

	if (buf == NULL || buf->data == NULL || buf->mapped) {
		GLERROR(GL_INVALID_OPERATION);
		return NULL;
	}

	if (access != GL_READ_ONLY_ARB)
		__pspgl_buffer_dlist_sync(buf->data);

	buf->access = access;
	buf->mapped = GL_TRUE;

	p = __pspgl_buffer_map(buf->data, access);

	psp_log("mapped buf %p data %p -> %p\n",
		buf, buf->data->base, p);

	return p;
}

GLvoid *glMapBuffer(GLenum target, GLenum access)
	__attribute__((alias("glMapBufferARB")));
