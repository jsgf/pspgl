#include "pspgl_internal.h"
#include "pspgl_buffers.h"

void glBindBufferARB(GLenum target, GLuint id)
{
	struct hashtable *hash = &pspgl_curctx->shared->buffers;
	struct pspgl_bufferobj *bufp, *prev, **prevp;

	prevp = __pspgl_bufferobj_for_target(target);

	if (prevp == NULL)
		return;

	prev = *prevp;

	bufp = NULL;
	if (id != 0)
		bufp = __pspgl_hash_lookup(hash, id);

	if (bufp != NULL) {
		if (bufp->target == 0)
			bufp->target = target;
		else if (bufp->target != target) {
			GLERROR(GL_INVALID_OPERATION);
			return;
		}
	} else if (id != 0) {
		bufp = __pspgl_bufferobj_new(target, NULL);
		if (bufp == NULL) {
			GLERROR(GL_OUT_OF_MEMORY);
			return;
		}
		__pspgl_hash_insert(hash, id, bufp);
	}

	if (bufp == *prevp)
		return;

	if (prev) {
		if (prev->mapped && prev->data)
			__pspgl_buffer_unmap(prev->data, prev->access);

		prev->mapped = GL_FALSE;
		__pspgl_bufferobj_free(prev);
	}

	if (bufp)
		bufp->refcount++;

	*prevp = bufp;
}

void glBindBuffer (GLenum, GLuint)
	__attribute__((alias("glBindBufferARB")));
