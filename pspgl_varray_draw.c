#include "pspgl_internal.h"
#include "pspgl_buffers.h"
#include "pspgl_dlist.h"

void __pspgl_varray_draw(GLenum mode, GLint first, GLsizei count)
{
	long prim;
	struct vertex_format vfmt, *vfmtp;
	struct pspgl_buffer *vbuf;
	unsigned vbuf_offset;
	const void *buf;

	prim = __pspgl_glprim2geprim(mode);
	if (prim < 0) {
		GLERROR(GL_INVALID_ENUM);
		return;
	}

	if (count == 0)
		return;

	vbuf = NULL;
	vbuf_offset = 0;
	vfmtp = &vfmt;

	/* Check to see if we can use the locked array fast path */
	if (__pspgl_cache_arrays()) {
		/* FAST: draw from locked array */
		struct locked_arrays *l = &pspgl_curctx->vertex_array.locked;

		vbuf = l->cached_array;
		vbuf_offset = l->cached_array_offset;
		vfmtp = &l->vfmt;
		first -= l->cached_first;

		vbuf->refcount++;
	}

	if (unlikely(vbuf == NULL)) {
		/* SLOW: convert us some arrays */
		vfmtp = &vfmt;
		__pspgl_ge_vertex_fmt(pspgl_curctx, &vfmt);

		if (unlikely(vfmt.hwformat == 0))
			return;

		vbuf = __pspgl_varray_convert(&vfmt, first, count);
		vbuf_offset = 0;

		if (unlikely(vbuf == NULL)) {
			GLERROR(GL_OUT_OF_MEMORY);
			return;
		}
	}

	buf = vbuf->base + vbuf_offset;
	buf += first * vfmtp->vertex_size;

	__pspgl_context_render_prim(pspgl_curctx, prim, count, vfmtp->hwformat, buf, NULL);
	__pspgl_dlist_pin_buffer(vbuf, BF_PINNED_RD);

	if (mode == GL_LINE_LOOP) {
		GLushort *idx = __pspgl_dlist_insert_space(sizeof(GLushort) * 2);

		assert(idx != NULL);

		idx[0] = first+count-1;
		idx[1] = first;

		psp_log("drawing closing line on loop: idx=%d %d\n", idx[0], idx[1]);
		__pspgl_context_render_prim(pspgl_curctx, GE_LINES, 2,
					    vfmtp->hwformat | GE_VINDEX_16BIT, buf, idx);
		__pspgl_dlist_pin_buffer(vbuf, BF_PINNED_RD);
	}

	__pspgl_buffer_free(vbuf);
}

