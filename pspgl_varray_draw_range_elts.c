#include <string.h>
#include <limits.h>

#include "pspgl_internal.h"
#include "pspgl_buffers.h"


/* Find the min and max indices in an element array.  Expects indices
   to be a mapped pointer. */
void __pspgl_find_minmax_indices(GLenum idx_type, const void *indices, unsigned count,
				 int *minidxp, int *maxidxp)
{
	/* find the min and max idx */
	int i;
	int minidx, maxidx;

	minidx = INT_MAX;
	maxidx = 0;

	switch(idx_type) {
	case GL_UNSIGNED_BYTE:
		for(i = 0; i < count; i++) {
			GLuint idx = ((GLubyte *)indices)[i];
			if (idx < minidx)
				minidx = idx;
			if (idx > maxidx)
				maxidx = idx;
		}
		break;

	case GL_UNSIGNED_SHORT:
		for(i = 0; i < count; i++) {
			GLuint idx = ((GLushort *)indices)[i];
			if (idx < minidx)
				minidx = idx;
			if (idx > maxidx)
				maxidx = idx;
		}
		break;

	case GL_UNSIGNED_INT:
		for(i = 0; i < count; i++) {
			GLuint idx = ((GLuint *)indices)[i];
			if (idx < minidx)
				minidx = idx;
			if (idx > maxidx)
				maxidx = idx;
		}
		break;
	}

	*minidxp = minidx;
	*maxidxp = maxidx;
}

/* 
   The hardware has direct support for drawing indexed vertex arrays,
   which we try hard to use.

   The best case is if the array has either been locked, or is in a
   VBO in hardware format.  In this case we can use the cached
   vertices directly with no conversion.  If the arrays are not in a
   VBO or are not in hardware format, then we need to allocate a
   transient buffer and put converted vertices into it.

   Indices are handled similarly; if they're in a buffer object in
   hardware format, then use them directly, otherwise convert them
   into a temporary buffer.

   In both cases we assume we can allocate enough memory for all
   vertices/indices.  I think that's a reasonable tradeoff to avoid
   the complexity of batching.
 */
void __pspgl_varray_draw_range_elts(GLenum mode, GLenum idx_type, 
				    const void *const indices, GLsizei count, 
				    int minidx, int maxidx)
{
	long prim;
	struct vertex_format vfmt;
	const struct vertex_format *vfmtp;
	struct pspgl_buffer *vbuf, *ibuf;
	unsigned vbuf_offset;
	unsigned idx_base;
	unsigned ibuf_offset;
	const void *vtxp, *idxp;
	unsigned hwformat;

	prim = __pspgl_glprim2geprim(mode);

	if (prim < 0) {
		GLERROR(GL_INVALID_ENUM);
		return;
	}

	if (count == 0)
		return;

	vbuf = NULL;
	idx_base = 0;

	if (__pspgl_cache_arrays()) {
		/* FAST: directly usable locked arrays */
		struct locked_arrays *l = &pspgl_curctx->vertex_array.locked;

		vbuf = l->cached_array;
		vbuf_offset = l->cached_array_offset;
		vfmtp = &l->vfmt;
		idx_base = l->cached_first;

		vbuf->refcount++;
	}

	if (vbuf == NULL) {
		/* SLOW: convert us some arrays */
		vfmtp = &vfmt;
		__pspgl_ge_vertex_fmt(pspgl_curctx, &vfmt);

		if (vfmt.hwformat == 0)
			return;

		if (minidx == -1 || maxidx == -1) {
			/* need to work out min/maxidx for ourselves */
			const void *idxmap;

			idxmap = __pspgl_bufferobj_map(pspgl_curctx->vertex_array.indexbuffer,
						       GL_READ_ONLY_ARB, (void *)indices);

			__pspgl_find_minmax_indices(idx_type, idxmap, count,
						    &minidx, &maxidx);

			__pspgl_bufferobj_unmap(pspgl_curctx->vertex_array.indexbuffer,
						GL_READ_ONLY_ARB);
		}

		vbuf = __pspgl_varray_convert(&vfmt, minidx, maxidx-minidx+1);
		vbuf_offset = 0;
		idx_base = minidx;

		if (vbuf == NULL) {
			GLERROR(GL_OUT_OF_MEMORY);
			return;
		}
	}

	hwformat = vfmtp->hwformat;

	ibuf = __pspgl_varray_convert_indices(idx_type, indices, idx_base, count,
					      &ibuf_offset, &hwformat);
	if (ibuf == NULL) {
		GLERROR(GL_OUT_OF_MEMORY);
		__pspgl_buffer_free(vbuf);
		return;
	}

	vtxp = vbuf->base + vbuf_offset;
	idxp = ibuf->base + ibuf_offset;

	__pspgl_context_render_prim(pspgl_curctx, prim, count, hwformat, vtxp, idxp);
	__pspgl_dlist_pin_buffer(vbuf, BF_PINNED_RD);
	__pspgl_dlist_pin_buffer(ibuf, BF_PINNED_RD);

	/* If we're drawing a line-loop, add an extra edge to close
	   the loop */
	if (mode == GL_LINE_LOOP) {
		GLushort *idx = __pspgl_dlist_insert_space(pspgl_curctx->dlist_current,
							   sizeof(GLushort) * 2);
		const void *idxmap;

		assert(idx != NULL);

		idxmap = __pspgl_buffer_map(ibuf, GL_READ_ONLY_ARB);

		if ((hwformat & GE_VINDEX_SHIFT(3)) == GE_VINDEX_8BIT) {
			idx[0] = ((GLubyte *)(idxmap+ibuf_offset))[count-1];
			idx[1] = ((GLubyte *)(idxmap+ibuf_offset))[0];
		} else {
			assert((hwformat & GE_VINDEX_SHIFT(3)) == GE_VINDEX_16BIT);

			idx[0] = ((GLushort *)(idxmap+ibuf_offset))[count-1];
			idx[1] = ((GLushort *)(idxmap+ibuf_offset))[0];
		}

		__pspgl_buffer_unmap(ibuf, GL_READ_ONLY_ARB);

		__pspgl_context_render_prim(pspgl_curctx, GE_LINES, 2,
					    hwformat, vtxp, idx);
		__pspgl_dlist_pin_buffer(vbuf, BF_PINNED_RD);
	}

	__pspgl_buffer_free(vbuf);
	__pspgl_buffer_free(ibuf);
}

void glDrawRangeElements( GLenum mode, GLuint start, GLuint end,
			  GLsizei count, GLenum type, const GLvoid *indices )
{
	__pspgl_varray_draw_range_elts(mode, type, indices, count, start, end);
}
