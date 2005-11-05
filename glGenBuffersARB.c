#include "pspgl_internal.h"
#include "pspgl_buffers.h"

void glGenBuffersARB (GLsizei n, GLuint *buffers)
{
	struct hashtable *hash = &pspgl_curctx->shared->buffers;
	int i;

	if (n < 0)
		GLERROR(GL_INVALID_VALUE);

	if (!buffers)
		return;

	for(i = 0; i < n; i++) {
		unsigned id;
		struct pspgl_bufferobj *bufp;

		id = __pspgl_hash_uniquekey(hash);

		bufp = __pspgl_bufferobj_new(NULL);
		__pspgl_hash_insert(hash, id, bufp);

		buffers[i] = id;
	}
}

void glGenBuffers (GLsizei count, GLuint *buffers)
	__attribute__((alias("glGenBuffersARB")));
