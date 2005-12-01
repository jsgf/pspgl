#include "pspgl_internal.h"
#include "pspgl_buffers.h"

void glDrawBezierRangeElementsPSP(GLenum mode, GLuint start, GLuint end,
				  GLuint u, GLuint v,
				  GLenum idx_type, const GLvoid *indices)
{
	struct pspgl_context *c = pspgl_curctx;
	long prim;
	struct vertex_format vfmt;
	const struct vertex_format *vfmtp;
	struct pspgl_buffer *vbuf, *ibuf;
	unsigned vbuf_offset;
	unsigned idx_base;
	unsigned ibuf_offset;
	const void *vtxp, *idxp;
	unsigned hwformat;
	unsigned count;
	int minidx = start;
	int maxidx = end;

	switch(mode) {
	case GL_TRIANGLES:	prim = 0; break;
	case GL_LINES:		prim = 1; break;
	case GL_POINTS:		prim = 2; break;

	default:
		GLERROR(GL_INVALID_ENUM);
		return;
	}
	sendCommandi(CMD_PATCH_PRIM, prim);

	count = u * v;

	if (count == 0)
		return;

	vbuf = NULL;
	idx_base = 0;

	if (__pspgl_cache_arrays()) {
		/* FAST: directly usable locked arrays */
		struct locked_arrays *l = &c->vertex_array.locked;

		vbuf = l->cached_array;
		vbuf_offset = l->cached_array_offset;
		vfmtp = &l->vfmt;
		idx_base = l->cached_first;

		vbuf->refcount++;
	}

	if (vbuf == NULL) {
		/* SLOW: convert us some arrays */
		vfmtp = &vfmt;
		__pspgl_ge_vertex_fmt(c, &vfmt);

		if (vfmt.hwformat == 0)
			return;

		if (minidx == -1 || maxidx == -1) {
			/* need to work out min/maxidx for ourselves */
			const void *idxmap;

			idxmap = __pspgl_bufferobj_map(c->vertex_array.indexbuffer,
						       GL_READ_ONLY_ARB, (void *)indices);

			__pspgl_find_minmax_indices(idx_type, idxmap, count,
						    &minidx, &maxidx);

			__pspgl_bufferobj_unmap(c->vertex_array.indexbuffer,
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

	__pspgl_context_render_setup(c, hwformat, vtxp, idxp);
	__pspgl_context_writereg_uncached(c, CMD_BEZIER, (v << 8) | u);
	__pspgl_context_pin_buffers(c);
	__pspgl_dlist_pin_buffer(vbuf, BF_PINNED_RD);
	__pspgl_dlist_pin_buffer(ibuf, BF_PINNED_RD);

	__pspgl_buffer_free(vbuf);
	__pspgl_buffer_free(ibuf);	
}

void glDrawBezierElementsPSP(GLenum mode, GLuint u, GLuint v, 
			     GLenum idx_type, const GLvoid *indices)
{
	struct locked_arrays *l = &pspgl_curctx->vertex_array.locked;
	int minidx, maxidx;

	minidx = maxidx = -1;

	if (l->count > 0) {
		/* use locked range */

		minidx = l->first;
		maxidx = l->first + l->count;
	} 

	glDrawBezierRangeElementsPSP(mode, minidx, maxidx, u, v, idx_type, indices);
}
