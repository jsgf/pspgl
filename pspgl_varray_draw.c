#include "pspgl_internal.h"

void __pspgl_varray_draw(GLenum mode, GLint first, GLsizei count)
{
	long prim = __pspgl_glprim2geprim(mode);
	struct vertex_format vfmt;
	GLint vtx0 = first;	/* used for batching if there's a fan */
	struct prim_info pi;
	unsigned maxbatch;

	__pspgl_ge_vertex_fmt(pspgl_curctx, &vfmt);

	if (prim < 0) {
		GLERROR(GL_INVALID_ENUM);
		return;
	}

	pi = __pspgl_prim_info[prim];

	if (count == 0 || vfmt.hwformat == 0)
		return;

	__pspgl_context_flush_pending_matrix_changes(pspgl_curctx);

	sendCommandi(CMD_VERTEXTYPE, vfmt.hwformat);

	maxbatch = MAX_VTX_BATCH / vfmt.vertex_size;

	while(count >= pi.minvtx) {
		unsigned batch = (count > maxbatch) ? maxbatch : count;
		unsigned batchsize;
		unsigned char *buf;
		unsigned done;

		batch = (batch / pi.vtxmult) * pi.vtxmult; /* round down to multiple */
		batchsize = batch * vfmt.vertex_size;
		buf = __pspgl_dlist_insert_space(pspgl_curctx->dlist_current, batchsize);

		if (prim == GE_TRIANGLE_FAN && first != vtx0) {
			done = __pspgl_gen_varray(&vfmt, vtx0, 1, buf, batchsize);
			done += __pspgl_gen_varray(&vfmt, first+1, count-1,
					      buf + vfmt.vertex_size,
					      batchsize - vfmt.vertex_size);
		} else
			done = __pspgl_gen_varray(&vfmt, first, count, buf, batchsize);

		count -= done;
		first += done;

		if ((count+pi.overlap) > pi.minvtx) {
			count += pi.overlap;
			first -= pi.overlap;
		} else {
			/* This is the last batch.  XXX deal with line loop? */
		}

		sendCommandiUncached(CMD_BASE, ((unsigned)buf >> 8) & 0xf0000);
		sendCommandiUncached(CMD_VERTEXPTR, ((unsigned)buf) & 0xffffff);
		sendCommandiUncached(CMD_PRIM, (prim << 16) | done);
	}

	/* XXX TODO: we handle line loops as line strips. Here we need to render the final, closing line, too. */
}

