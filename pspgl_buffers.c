#include <stdlib.h>
#include <psputils.h>

#include "pspgl_internal.h"
#include "pspgl_buffers.h"

struct pspgl_bufferobj *__pspgl_bufferobj_new(GLenum target, struct pspgl_buffer *data)
{
	struct pspgl_bufferobj *bufp;

	bufp = malloc(sizeof(*bufp));

	if (bufp) {
		bufp->refcount = 1;
		bufp->target = target;
		bufp->usage = 0;
		bufp->mapped = GL_FALSE;

		bufp->data = data;
		if (data != NULL)
			data->refcount++;
	}

	return bufp;
}

void __pspgl_bufferobj_free(struct pspgl_bufferobj *bufp)
{
	assert(bufp->refcount > 0);

	if (--bufp->refcount)
		return;

	if (bufp->data)
		__pspgl_buffer_free(bufp->data);

	free(bufp);
}

void *__pspgl_bufferobj_deref(const struct pspgl_bufferobj *bufp, void *off)
{
	if (bufp == NULL || bufp->data == NULL)
		return off;

	return bufp->data->base + (off - NULL);
}

struct pspgl_buffer *__pspgl_buffer_new(void *base, GLsizeiptr size,
						void (*free)(struct pspgl_buffer *))
{
	struct pspgl_buffer *ret;

	ret = malloc(sizeof(*ret));

	if (ret != NULL) {
		ret->refcount = 1;
		ret->base = base;
		ret->size = size;
	}

	return ret;
}

void __pspgl_buffer_free(struct pspgl_buffer *data)
{
	assert(data->refcount > 0);

	if (--data->refcount)
		return;

	if (data->free)
		(*data->free)(data);

	free(data);
}

struct pspgl_bufferobj **__pspgl_bufferobj_for_target(GLenum target)
{
	struct pspgl_bufferobj **ret = NULL;

	switch(target) {
	default:
		GLERROR(GL_INVALID_ENUM);
		break;
	}

	return ret;
}

void *__pspgl_bufferobj_map(struct pspgl_bufferobj *buf, GLenum access)
{
	void *p = buf->data->base;

	switch(access) {
	case GL_READ_WRITE_ARB:
		/* FALLTHROUGH */
	case GL_READ_ONLY_ARB:
		/* need to invalidate if written by hardware */
		sceKernelDcacheInvalidateRange(buf->data->base, buf->data->size);
		break;

	case GL_WRITE_ONLY_ARB:
		/* Write-only streams can be uncached to prevent cache
		   pollution */
		p = __pspgl_uncached(p, buf->data->size);
		break;
	}

	return p;
}

void __pspgl_bufferobj_unmap(struct pspgl_bufferobj *buf, GLenum access)
{
	switch(access) {
	case GL_READ_ONLY_ARB:
		/* do nothing; no dirty cache lines */
		break;

	case GL_READ_WRITE_ARB:
		sceKernelDcacheWritebackInvalidateRange(buf->data->base, buf->data->size);
		break;

	case GL_WRITE_ONLY_ARB:
		/* do nothing; all uncached */
		break;
	}
}
