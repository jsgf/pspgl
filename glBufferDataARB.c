#include <string.h>
#include <stdlib.h>
#include <malloc.h>

#include "pspgl_internal.h"
#include "pspgl_buffers.h"

static void data_free_heap(struct pspgl_buffer *data)
{
	free(data->base);
}

void glBufferDataARB(GLenum target, GLsizeiptr size,
		     const GLvoid *init_data, GLenum usage)
{
	struct pspgl_bufferobj *buf, **bufp;
	struct pspgl_buffer *databuf;
	GLsizeiptr allocsize;
	void *data;

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


	/* XXX TODO
	   
	   Use "usage" to determine where the buffer is
	   allocated. COPY means GL->GL data, READ means
	   GL->application, DRAW means app->GL.
	   Hm, target can also play a role here...

	   Initial thoughts:
	   *_COPY		VRAM (by necessity?)
	   STATIC_*		VRAM (hardware uses data more)
	   STREAM+DYNAMIC	system memory?
	 */

	allocsize = ROUNDUP(size, CACHELINE_SIZE);

	/* cache-line aligned for easy uncached access later */
	data = memalign(CACHELINE_SIZE, allocsize);

	if (data == NULL) {
		GLERROR(GL_OUT_OF_MEMORY);
		return;
	}

	databuf = __pspgl_buffer_new(data, allocsize, data_free_heap);

	if (databuf == NULL) {
		free(data);
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
