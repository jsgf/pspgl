#include <string.h>

#include "pspgl_internal.h"

static unsigned getidx(const void *elts, GLenum idx_type, unsigned idx)
{
	unsigned ret = 0;

	switch(idx_type) {
	case GL_UNSIGNED_BYTE:
		ret = ((GLubyte *)elts)[idx];
		break;

	case GL_UNSIGNED_SHORT:
		ret = ((GLushort *)elts)[idx];
		break;

	case GL_UNSIGNED_INT:
		ret = ((GLuint *)elts)[idx];
		break;
	}

	return ret;
}


/* Like gen_varray(), but the vertices are indexed by the "elts" array */
static int gen_varray_elts(const struct vertex_format *vfmt, int first, int count, 
			   GLenum idx_type, const void *elts, void *to, int space)
{
	int i;
	unsigned char *dest = to;
	int nvtx = space / vfmt->vertex_size;

	if (nvtx > count)
		nvtx = count;

	for(i = 0; i < nvtx; i++) {
		int j;
		unsigned idx = getidx(elts, idx_type, first + i);

		for(j = 0; j < vfmt->nattrib; j++) {
			const struct attrib *attr = &vfmt->attribs[j];
			struct pspgl_vertex_array *a = attr->array;
			const unsigned char *ptr;

			ptr = a->ptr + (idx * a->stride);

			if (attr->convert)
				(*attr->convert)(&dest[attr->offset], ptr, attr);
			else
				memcpy(&dest[attr->offset], ptr, attr->size);
		}
		dest += vfmt->vertex_size;
	}

	return nvtx;
}


static void draw_range_elts_fallback(unsigned prim, GLenum idx_type, const void *indices, 
				     GLsizei count, const struct vertex_format *vfmt)
{
	struct prim_info pi = __pspgl_prim_info[prim];
	unsigned first = 0;
	unsigned maxbatch;

	sendCommandi(CMD_VERTEXTYPE, vfmt->hwformat);

	maxbatch = MAX_VTX_BATCH / vfmt->vertex_size;

	psp_log("prim=%d maxbatch=%d\n", prim, maxbatch);

	while(count >= pi.minvtx) {
		unsigned batch = (count > maxbatch) ? maxbatch : count;
		unsigned batchsize;
		unsigned char *buf;
		unsigned done;

		batch = (batch / pi.vtxmult) * pi.vtxmult; /* round down to multiple */
		batchsize = batch * vfmt->vertex_size;
		buf = __pspgl_dlist_insert_space(pspgl_curctx->dlist_current, batchsize);

		psp_log("first=%d count=%d batch=%d batchsize=%d buf=%p\n", 
			first, count, batch, batchsize, buf);

		if (buf == NULL)
			break;

		if (prim == GE_TRIANGLE_FAN && first != 0) {
			done = gen_varray_elts(vfmt, 0, 1,
					       idx_type, indices,
					       buf, batchsize);
			done += gen_varray_elts(vfmt, first+1, count-1,
						idx_type, indices,
						buf + vfmt->vertex_size,
						batchsize - vfmt->vertex_size);
		} else
			done = gen_varray_elts(vfmt, first, count, 
					       idx_type, indices, buf, batchsize);

		count -= done;
		first += done;

		if ((count+pi.overlap) > pi.minvtx) {
			count += pi.overlap;
			first -= pi.overlap;
		} else {
			/* This is the last batch.  XXX deal with line loop? */
		}

		__pspgl_context_flush_pending_matrix_changes(pspgl_curctx);
		sendCommandi(CMD_VERTEXTYPE, vfmt->hwformat);
		sendCommandiUncached(CMD_BASE, ((unsigned)buf >> 8) & 0xf0000);
		sendCommandiUncached(CMD_VERTEXPTR, ((unsigned)buf) & 0xffffff);
		sendCommandiUncached(CMD_PRIM, (prim << 16) | done);
	}
}


/* Try to use the hardware fast-path for drawing an indexed vertex
   array. All the vertices and indices are copied into the vertex
   buffer, and are drawn with a single operation.  If we can't do that
   for some reason (they won't all fit in the buffer, say), then the
   fallback has to do it the hard way. */
void __pspgl_varray_draw_range_elts(GLenum mode, GLenum idx_type, const void *indices, GLsizei count, 
			       unsigned minidx, unsigned maxidx)
{
	long prim = __pspgl_glprim2geprim(mode);
	struct vertex_format vfmt;
	struct prim_info pi;
	unsigned maxbatch;
	unsigned numvtx = maxidx - minidx + 1;
	unsigned vtxsize, idxsize;
	unsigned hwformat;

	__pspgl_ge_vertex_fmt(pspgl_curctx, &vfmt);

	if (prim < 0) {
		GLERROR(GL_INVALID_ENUM);
		return;
	}

	pi = __pspgl_prim_info[prim];

	if (count == 0 || vfmt.hwformat == 0)
		return;

	vtxsize = ROUNDUP(numvtx * vfmt.vertex_size, 16);

	switch(idx_type) {
	case GL_UNSIGNED_BYTE:	idxsize = 1; break;
	case GL_UNSIGNED_SHORT:
	case GL_UNSIGNED_INT:	idxsize = 2; break;
	default:		idxsize = 0;
	}

	if (idxsize == 0) {
		GLERROR(GL_INVALID_ENUM);
		return;
	}

	idxsize *= count;

	maxbatch = (MAX_VTX_BATCH - idxsize - 16) / vfmt.vertex_size;

	hwformat = vfmt.hwformat;

	psp_log("prim=%d maxbatch=%d numvtx=%d count=%d maxidx=%d minidx=%d\n",
		prim, maxbatch, numvtx, count, maxidx, minidx);

	if (numvtx < maxbatch) {
		/* fastpath: offload everything in one step */
		unsigned i;
		unsigned char *buf = __pspgl_dlist_insert_space(pspgl_curctx->dlist_current, 
								vtxsize + idxsize);
		unsigned char *vtxbuf = buf;
		void *idxbuf = buf+vtxsize;
		unsigned done;

		if (buf)
			done = __pspgl_gen_varray(&vfmt, minidx, numvtx, vtxbuf, vtxsize);

		if (!buf || done != numvtx) {
			/* couldn't use fast path */
			draw_range_elts_fallback(prim, idx_type, indices, count, &vfmt);
			return;
		}

		switch(idx_type) {
		case GL_UNSIGNED_BYTE:
			hwformat |= GE_VINDEX_8BIT;
			for(i = 0; i < count; i++) 
				((GLubyte *)idxbuf)[i] = ((GLubyte *)indices)[i];
			break;

		case GL_UNSIGNED_SHORT:
			hwformat |= GE_VINDEX_16BIT;
			for(i = 0; i < count; i++)
				((GLushort *)idxbuf)[i] = ((GLushort *)indices)[i];
			break;

		case GL_UNSIGNED_INT:
			hwformat |= GE_VINDEX_16BIT;
			for(i = 0; i < count; i++)
				((GLushort *)idxbuf)[i] = ((GLuint *)indices)[i];
			break;

		}

		__pspgl_context_flush_pending_matrix_changes(pspgl_curctx);
		sendCommandi(CMD_VERTEXTYPE, hwformat);

		sendCommandiUncached(CMD_BASE, ((unsigned)vtxbuf >> 8) & 0xf0000);
		sendCommandiUncached(CMD_VERTEXPTR, ((unsigned)vtxbuf) & 0xffffff);
		sendCommandiUncached(CMD_BASE, ((unsigned)idxbuf >> 8) & 0xf0000);
		sendCommandiUncached(CMD_INDEXPTR, ((unsigned)idxbuf) & 0xffffff);

		sendCommandiUncached(CMD_PRIM, (prim << 16) | count);
	} else
		draw_range_elts_fallback(prim, idx_type, indices, count, &vfmt);
}
