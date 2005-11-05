#include "pspgl_internal.h"
#include "pspgl_buffers.h"

static void varray_draw_locked(GLenum mode, GLint first, GLsizei count)
{
	struct locked_arrays *l = &pspgl_curctx->vertex_array.locked;
	long prim = __pspgl_glprim2geprim(mode);
	unsigned long buf;

	first -= l->cached_first;

	psp_log("mode=%d drawing %d-%d vertces from locked buffer\n",
		mode, first, first+count);

	buf = (unsigned long)l->cached_array->base + l->cached_array_offset;
	buf += first * l->vfmt.vertex_size;

	sendCommandi(CMD_VERTEXTYPE, l->vfmt.hwformat);

	sendCommandiUncached(CMD_BASE, ((unsigned)buf >> 8) & 0xf0000);
	sendCommandiUncached(CMD_VERTEXPTR, ((unsigned)buf) & 0xffffff);
	sendCommandiUncached(CMD_PRIM, (prim << 16) | count);

	__pspgl_buffer_dlist_use(l->cached_array);

	/* If we're drawing a line-loop, draw the final edge
	   XXX this pulls in all the indexed array code
	 */
	if (mode == GL_LINE_LOOP) {
		GLushort idx[2] = { first+count-1, first };
		struct pspgl_bufferobj *idxbuf;

		/* temporarily set no index buffer */
		idxbuf = pspgl_curctx->vertex_array.indexbuffer;
		pspgl_curctx->vertex_array.indexbuffer = NULL;

		psp_log("drawing closing line on loop: idx=%d %d\n", idx[0], idx[1]);

		__pspgl_varray_draw_range_elts(GL_LINES, GL_UNSIGNED_SHORT, idx, 2,
					       idx[1], idx[0]);

		pspgl_curctx->vertex_array.indexbuffer = idxbuf;
	}
}

void __pspgl_varray_draw(GLenum mode, GLint first, GLsizei count)
{
	long prim = __pspgl_glprim2geprim(mode);
	struct vertex_format vfmt;
	const GLint vtx0 = first;	/* used for batching if there's a fan */
	struct prim_info pi;
	unsigned maxbatch;
	GLsizei remains;

	__pspgl_ge_vertex_fmt(pspgl_curctx, &vfmt);

	if (prim < 0) {
		GLERROR(GL_INVALID_ENUM);
		return;
	}

	pi = __pspgl_prim_info[prim];

	if (count == 0)
		return;

	__pspgl_context_flush_pending_matrix_changes(pspgl_curctx);

	/* Check to see if we can use the locked array fast path */
	if (__pspgl_cache_arrays()) {
		/* FAST: draw from locked array */
		varray_draw_locked(mode, first, count);
		return;
	}

	__pspgl_ge_vertex_fmt(pspgl_curctx, &vfmt);

	pi = __pspgl_prim_info[prim];

	if (vfmt.hwformat == 0)
		return;

	sendCommandi(CMD_VERTEXTYPE, vfmt.hwformat);

	maxbatch = MAX_VTX_BATCH / vfmt.vertex_size;

	remains = count;
	while(remains >= pi.minvtx) {
		unsigned batch = (remains > maxbatch) ? maxbatch : remains;
		unsigned batchsize;
		unsigned char *buf;
		unsigned done;

		batch = (batch / pi.vtxmult) * pi.vtxmult; /* round down to multiple */
		batchsize = batch * vfmt.vertex_size;
		buf = __pspgl_dlist_insert_space(pspgl_curctx->dlist_current, batchsize);

		if (prim == GE_TRIANGLE_FAN && first != vtx0) {
			done = __pspgl_gen_varray(&vfmt, vtx0, 1, buf, batchsize);
			done += __pspgl_gen_varray(&vfmt, first+1, remains-1,
						   buf + vfmt.vertex_size,
						   batchsize - vfmt.vertex_size);
		} else
			done = __pspgl_gen_varray(&vfmt, first, remains, buf, batchsize);

		remains -= done;
		first += done;

		if ((remains+pi.overlap) > pi.minvtx) {
			remains += pi.overlap;
			first -= pi.overlap;
		}

		sendCommandiUncached(CMD_BASE, ((unsigned)buf >> 8) & 0xf0000);
		sendCommandiUncached(CMD_VERTEXPTR, ((unsigned)buf) & 0xffffff);
		sendCommandiUncached(CMD_PRIM, (prim << 16) | done);
	}

	/* If we're drawing a line-loop, draw the final edge
	   XXX this pulls in all the indexed array code
	*/
	if (mode == GL_LINE_LOOP) {
		GLushort idx[2] = { vtx0+count-1, vtx0 };
		struct pspgl_bufferobj *idxbuf;

		/* temporarily set no index buffer */
		idxbuf = pspgl_curctx->vertex_array.indexbuffer;
		pspgl_curctx->vertex_array.indexbuffer = NULL;

		psp_log("drawing closing line on loop: idx=%d %d\n", idx[0], idx[1]);

		__pspgl_varray_draw_range_elts(GL_LINES, GL_UNSIGNED_SHORT, idx, 2,
					       idx[1], idx[0]);

		pspgl_curctx->vertex_array.indexbuffer = idxbuf;
	}
}

