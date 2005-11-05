#include "pspgl_internal.h"
#include "pspgl_buffers.h"

void glDeleteBuffersARB (GLsizei n, const GLuint *buffers)
{
	int i;
	struct hashtable *hash = &pspgl_curctx->shared->buffers;

	if (n < 0) {
		GLERROR(GL_INVALID_VALUE);
		return;
	}

	for(i = 0; i < n; i++) {
		GLuint id  = buffers[i];
		struct pspgl_bufferobj *bufp;

		if (id == 0)
			continue;

		bufp = __pspgl_hash_remove(hash, id);

		if (bufp) {
			struct pspgl_bufferobj **boundp;

			boundp = __pspgl_bufferobj_for_target(bufp->target);

			if (boundp != NULL && *boundp == bufp)
				glBindBufferARB(bufp->target, 0);

			__pspgl_bufferobj_free(bufp);
		}
	}
}

void glDeleteBuffers (GLsizei n, const GLuint *buffers)
	__attribute__((alias("glDeleteBuffersARB")));
