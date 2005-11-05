#include <stdlib.h>
#include <malloc.h>
#include <pspge.h>
#include <psputils.h>

#include "pspgl_internal.h"
#include "pspgl_buffers.h"

/* Maximum size of cached vertices */
#define MAX_CACHED_ARRAY	(128*1024)

static void vidmem_free(struct pspgl_buffer *data)
{
	__pspgl_vidmem_free(data->base);
}

static void heap_free(struct pspgl_buffer *data)
{
	free(data->base);
}

static struct pspgl_buffer *alloc_array_buffer(unsigned bytes)
{
	void *p;
	void (*freep)(struct pspgl_buffer *);
	struct pspgl_buffer *data;

	/* Try placing cached array in EDRAM; seems to be good for
	   about 20% performance improvement. */
	p = __pspgl_vidmem_alloc(bytes);
	freep = vidmem_free;

	if (p == NULL) {
		p = memalign(16, bytes);
		freep = heap_free;
	}

	if (p == NULL)
		return NULL;

	data = __pspgl_buffer_new(p, bytes, freep);

	if (data == NULL) {
		struct pspgl_buffer d = { .base = p };
		(*freep)(&d);
	}

	return data;
}

/* Cache current arrays into a buffer in hardware form, if possible.
   Could fail for any number of reasons; returns true if it
   succeeds. */
GLboolean __pspgl_cache_arrays(void)
{
	struct locked_arrays *l = &pspgl_curctx->vertex_array.locked;
	struct pspgl_buffer *cached;
	unsigned size;
	int count;

	if (l->count == 0) {
		psp_log("failed: not locked\n");
		return GL_FALSE; /* nothing locked */
	}

	if (l->cached_array != NULL) {
		psp_log("OK: already cached\n");
		return GL_TRUE;
	}

	__pspgl_ge_vertex_fmt(pspgl_curctx, &l->vfmt);

	size = l->vfmt.vertex_size * l->count;

	psp_log("caching arrays %d-%d, vfmt=%x arrays=%x, %d bytes\n",
		l->first, l->first+l->count, l->vfmt.hwformat, l->vfmt.arrays, size);

	if (size == 0 || size >= MAX_CACHED_ARRAY) {
		psp_log("failed: size=%d\n", size);
		return GL_FALSE; /* too small or too big */
	}

	cached = alloc_array_buffer(size);
	if (cached == NULL) {
		psp_log("failed: out of memory\n");
		return GL_FALSE; /* out of memory */
	}

	l->cached_array = cached;

	void *p = __pspgl_buffer_map(cached, GL_WRITE_ONLY_ARB);

	count = __pspgl_gen_varray(&l->vfmt, l->first, l->count, p, size);

	__pspgl_buffer_unmap(cached, GL_WRITE_ONLY_ARB);

	if (count != l->count) {
		psp_log("failed: wanted %d vertices, only got %d\n", l->count, count);
		__pspgl_uncache_arrays();
		return GL_FALSE; /* conversion failed */
	}

	psp_log("OK: vertex arrays cached\n");

	return GL_TRUE;
}

void glLockArraysEXT(GLint first, GLsizei count)
{
	struct locked_arrays *l = &pspgl_curctx->vertex_array.locked;

	if (first < 0 || count <= 0) {
		GLERROR(GL_INVALID_VALUE);
		return;
	}

	if (l->count != 0) {
		GLERROR(GL_INVALID_OPERATION);
		return;
	}

	l->first = first;
	l->count = count;

	psp_log("locking arrays %d %d\n", first, count);

	/* defer actually caching things until first use of arrays */
}

/* Clear out the cached array (if any), but doesn't change the locked state */
void __pspgl_uncache_arrays(void)
{
	struct locked_arrays *l = &pspgl_curctx->vertex_array.locked;

	if (l->cached_array)
		__pspgl_buffer_free(l->cached_array);

	l->cached_array = NULL;
	l->vfmt.hwformat = 0;
	l->vfmt.vertex_size = 0;
	l->vfmt.arrays = 0;
}

void glUnlockArraysEXT(void)
{
	struct locked_arrays *l = &pspgl_curctx->vertex_array.locked;

	if (l->count == 0) {
		GLERROR(GL_INVALID_OPERATION);
		return;
	}

	l->first = l->count = 0;

	__pspgl_uncache_arrays();
}
