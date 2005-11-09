#include <stdlib.h>
#include <malloc.h>

#include <psputils.h>
#include <pspge.h>

#include "pspgl_internal.h"
#include "pspgl_buffers.h"

struct pspgl_bufferobj *__pspgl_bufferobj_new(struct pspgl_buffer *data)
{
	struct pspgl_bufferobj *bufp;

	bufp = malloc(sizeof(*bufp));

	if (bufp) {
		bufp->refcount = 1;
		bufp->usage = 0;
		bufp->access = 0;
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

void *__pspgl_bufferobj_map(const struct pspgl_bufferobj *buf, GLenum access, void *off)
{
	if (buf && buf->data)
		off += __pspgl_buffer_map(buf->data, access) - NULL;

	return off;
}

void __pspgl_bufferobj_unmap(const struct pspgl_bufferobj *buf, GLenum access)
{
	if (buf && buf->data)
		__pspgl_buffer_unmap(buf->data, access);
}

static int is_edram_addr(void *p)
{
	static void *edram_start, *edram_end;

	if (edram_end == NULL) {
		edram_start = sceGeEdramGetAddr();
		edram_end = sceGeEdramGetAddr() + sceGeEdramGetSize();
	}

	return (p >= edram_start) && (p < edram_end);
}

GLboolean __pspgl_buffer_init(struct pspgl_buffer *buf,
			      GLsizeiptr size, GLenum usage)
{
	void *p = NULL;

	size = ROUNDUP(size, CACHELINE_SIZE);

	switch(usage) {
	case GL_STATIC_DRAW_ARB:	/* nice to have in edram */
	case GL_STATIC_READ_ARB:
	case GL_DYNAMIC_READ_ARB:

	case GL_STATIC_COPY_ARB:	/* must be in edram */
	case GL_DYNAMIC_COPY_ARB:
	case GL_STREAM_COPY_ARB:
		p = __pspgl_vidmem_alloc(size);
		break;

	case GL_DYNAMIC_DRAW_ARB:	/* prefer in system memory */
	case GL_STREAM_READ_ARB:
	case GL_STREAM_DRAW_ARB:
		/* fallthrough to allocation */
		break;

	default:
		GLERROR(GL_INVALID_ENUM);
		return GL_FALSE;
	}

	if (p == NULL) {
		p = memalign(CACHELINE_SIZE, size);

		if (p == NULL)
			return GL_FALSE;
	}

  	/* put cache into appropriate unmapped state */
 	sceKernelDcacheWritebackInvalidateRange(p, size);

	buf->refcount = 1;
	buf->mapped = 0;
	buf->flags = 0;
	buf->generation = 0;

	buf->pin_prevp = NULL;
	buf->pin_next = NULL;

	buf->base = p;
	buf->size = size;

	return GL_TRUE;
}

struct pspgl_buffer *__pspgl_buffer_new(GLsizeiptr size, GLenum usage)
{
	struct pspgl_buffer *ret;

	ret = malloc(sizeof(*ret));

	if (ret != NULL)
		if (!__pspgl_buffer_init(ret, size, usage)) {
			free(ret);
			ret = NULL;
		}

	return ret;
}

void __pspgl_buffer_free(struct pspgl_buffer *data)
{
	assert(data->refcount > 0);

	if (--data->refcount)
		return;

	if (data->base) {
		if (is_edram_addr(data->base))
			__pspgl_vidmem_free(data->base);
		else
			free(data->base);
	}
	free(data);
}

struct pspgl_bufferobj **__pspgl_bufferobj_for_target(GLenum target)
{
	struct pspgl_bufferobj **ret = NULL;

	switch(target) {
	case GL_ARRAY_BUFFER_ARB:
		ret = &pspgl_curctx->vertex_array.arraybuffer;
		break;

	case GL_ELEMENT_ARRAY_BUFFER_ARB:
		ret = &pspgl_curctx->vertex_array.indexbuffer;
		break;

	case GL_PIXEL_PACK_BUFFER_ARB:
		ret = &pspgl_curctx->texture.packbuffer;
		break;

	case GL_PIXEL_UNPACK_BUFFER_ARB:
		ret = &pspgl_curctx->texture.unpackbuffer;
		break;

	default:
		GLERROR(GL_INVALID_ENUM);
		break;
	}

	return ret;
}

void *__pspgl_buffer_map(struct pspgl_buffer *data, GLenum access)
{
	void *p = data->base;

	assert(data->mapped >= 0);

	switch(access) {
	case GL_READ_WRITE_ARB:
		/* FALLTHROUGH */

	case GL_READ_ONLY_ARB:
		/* Need to invalidate if written by hardware, but only the first time */
		if (!data->mapped)
			sceKernelDcacheInvalidateRange(data->base, data->size);
		break;

	case GL_WRITE_ONLY_ARB:
		/* Write-only streams are uncached to prevent cache
		   pollution.  If data->mapped != 0, this should be a
		   no-op. */
		p = __pspgl_uncached(p, data->size);
		break;

	default:
		GLERROR(GL_INVALID_ENUM);
		return NULL;
	}

	data->mapped++;

	return p;
}

void __pspgl_buffer_unmap(struct pspgl_buffer *data, GLenum access)
{
	assert(data->mapped > 0);

	if (--data->mapped > 0)
		return;

	switch(access) {
	case GL_READ_ONLY_ARB:
		/* do nothing; no dirty cache lines */
		break;

	case GL_READ_WRITE_ARB:
		sceKernelDcacheWritebackInvalidateRange(data->base, data->size);
		break;

	case GL_WRITE_ONLY_ARB:
		/* do nothing; all uncached */
		break;

	default:
		return;
	}

	if (access != GL_READ_ONLY_ARB)
		data->generation++;
}

/* Wait until the last hardware use of a databuffer has happened. If
   the caller wants to use the buffer after this call, it must
   increment the refcount to prevent it from being (potentially)
   freed. */
void __pspgl_buffer_dlist_sync(struct pspgl_buffer *data)
{
	data->refcount++;	/* prevent freeing */

	/* XXX This is overkill; we can wait for each dlist until this
	   buffer stops being pinned. */
	if (data->flags & BF_PINNED)
		glFinish();

	assert((data->flags & BF_PINNED) == 0);

	__pspgl_buffer_free(data); /* drop refcount */
}
