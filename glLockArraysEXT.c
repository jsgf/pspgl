#include <stdlib.h>
#include <malloc.h>
#include <pspge.h>
#include <psputils.h>

#include "pspgl_internal.h"

/* Maximum size of cached vertices */
#define MAX_CACHED_ARRAY	(128*1024)

static struct array_buffer *alloc_array_buffer(unsigned bytes)
{
	struct array_buffer *b = malloc(sizeof(*b));

	if (b == NULL)
		return NULL;

	/* Try placing cached array in EDRAM; seems to be good for
	   about 20% performance improvement. */
	b->array = __pspgl_vidmem_alloc(bytes);

	if (b->array == NULL)
		b->array = memalign(16, bytes);

	if (b->array == NULL) {
		free(b);
		return NULL;
	}

	b->refcount = 1;

	return b;
}

static int is_edram_addr(void *p)
{
	static void *edram_start, *edram_end;
	static unsigned edram_size;

	if (edram_size == 0) {
		edram_start = sceGeEdramGetAddr();
		edram_end = edram_start + sceGeEdramGetSize();
	}

	return edram_start <= p && p < edram_end;
}

static void free_array_buffer(struct array_buffer *b)
{
	assert(b->refcount > 0);

	if (--b->refcount)
		return;

	if (is_edram_addr(b->array))
		__pspgl_vidmem_free(b->array);
	else
		free(b->array);
	free(b);
}

void __pspgl_dlist_cleanup_varray(void *v)
{
	struct array_buffer *b = v;

	psp_log("b(%p)->refcount=%d\n", b, b->refcount);

	free_array_buffer(b);
}

/* Cache current arrays into a buffer in hardware form, if possible.
   Could fail for any number of reasons; returns true if it
   succeeds. */
GLboolean __pspgl_cache_arrays(void)
{
	struct locked_arrays *l = &pspgl_curctx->vertex_array.locked;
	struct array_buffer *cached;
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

	count = __pspgl_gen_varray(&l->vfmt, l->first, l->count, cached->array, size);
	if (count != l->count) {
		psp_log("failed: wanted %d vertices, only got %d\n", l->count, count);
		__pspgl_uncache_arrays();
		return GL_FALSE; /* conversion failed */
	}

	sceKernelDcacheWritebackInvalidateRange(cached->array, size);

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
		free_array_buffer(l->cached_array);

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
