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

/*
   Fallback indexed vertex rendering.  We can't use the hardware
   indexing, so we need to manually index each vertex and emit
   vertices into the command buffer memory.
 */
static void draw_range_elts_fallback(GLenum mode, GLenum idx_type, const void *indices, 
				     const GLsizei count, const struct vertex_format *vfmt)
{
	long prim = __pspgl_glprim2geprim(mode);
	struct prim_info pi = __pspgl_prim_info[prim];
	unsigned first = 0;
	unsigned maxbatch;
	GLsizei remains;

	sendCommandi(CMD_VERTEXTYPE, vfmt->hwformat);

	maxbatch = MAX_VTX_BATCH / vfmt->vertex_size;

	psp_log("prim=%d maxbatch=%d\n", prim, maxbatch);

	remains = count;
	while(remains >= pi.minvtx) {
		unsigned batch = (remains > maxbatch) ? maxbatch : remains;
		unsigned batchsize;
		unsigned char *buf;
		unsigned done;

		batch = (batch / pi.vtxmult) * pi.vtxmult; /* round down to multiple */
		batchsize = batch * vfmt->vertex_size;
		buf = __pspgl_dlist_insert_space(pspgl_curctx->dlist_current, batchsize);

		psp_log("first=%d remains=%d batch=%d batchsize=%d buf=%p\n", 
			first, remains, batch, batchsize, buf);

		if (buf == NULL)
			break;

		if (prim == GE_TRIANGLE_FAN && first != 0) {
			done = gen_varray_elts(vfmt, 0, 1,
					       idx_type, indices,
					       buf, batchsize);
			done += gen_varray_elts(vfmt, first+1, remains-1,
						idx_type, indices,
						buf + vfmt->vertex_size,
						batchsize - vfmt->vertex_size);
		} else
			done = gen_varray_elts(vfmt, first, remains, 
					       idx_type, indices, buf, batchsize);

		remains -= done;
		first += done;

		if ((remains+pi.overlap) > pi.minvtx) {
			remains += pi.overlap;
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
	/* Go recursive to draw the final edge of a line loop */
	if (mode == GL_LINE_LOOP) {
		GLushort idx[2] = { count-1, 0 };

		draw_range_elts_fallback(GL_LINES, GL_UNSIGNED_SHORT, idx, 2, vfmt);
	}
}

static unsigned convert_indices(void *to, const void *from, GLenum idx_type, 
				int offset, GLsizei count)
{
	unsigned hwformat = 0;
	unsigned i;

	switch(idx_type) {
	case GL_UNSIGNED_BYTE:
		hwformat = GE_VINDEX_8BIT;
		for(i = 0; i < count; i++) 
			((GLubyte *)to)[i] = ((GLubyte *)from)[i] - offset;
		break;

	case GL_UNSIGNED_SHORT:
		hwformat = GE_VINDEX_16BIT;
		for(i = 0; i < count; i++)
			((GLushort *)to)[i] = ((GLushort *)from)[i] - offset;
		break;

	case GL_UNSIGNED_INT:
		hwformat = GE_VINDEX_16BIT;
		for(i = 0; i < count; i++)
			((GLushort *)to)[i] = ((GLuint *)from)[i] - offset;
		break;

	}

	return hwformat;
}

static int idx_sizeof(GLenum idx_type)
{
	int idxsize;

	switch(idx_type) {
	case GL_UNSIGNED_BYTE:	idxsize = 1; break;
	case GL_UNSIGNED_SHORT:
	case GL_UNSIGNED_INT:	idxsize = 2; break;
	default:		idxsize = 0;
	}

	return idxsize;
}

/* 
   We have a locked vertex array, so try to directly use it.  This
   means that we don't have to do any vertex format conversion here;
   we only need to copy the index data into the command buffer.

   This may fall back to the slow path if we can't allocate enough
   buffer for the index data.
 */
static void draw_range_elts_locked(GLenum mode, GLenum idx_type, const void *indices, 
				   GLsizei count)
{
	struct locked_arrays *l = &pspgl_curctx->vertex_array.locked;
	unsigned extra;
	unsigned prim;
	void *idxbuf;
	unsigned hwformat;
	void *vtxbuf;
	unsigned idxsize;

	prim = __pspgl_glprim2geprim(mode);

	idxsize = idx_sizeof(idx_type);

	if (idxsize == 0) {
		GLERROR(GL_INVALID_ENUM);
		return;
	}

	psp_log("drawing locked vertices: mode=%d idxtype=%x count=%d\n",
		mode, idx_type, count);

	/* If this is a line loop, add an extra index for the final
	   edge */
	extra = 0;
	if (mode == GL_LINE_LOOP)
		extra = 1;

	idxbuf = __pspgl_dlist_insert_space(pspgl_curctx->dlist_current, 
					    idxsize * (count + extra));

	if (idxbuf == NULL) {
		struct vertex_format vfmt;

		/* SLOW: index data won't fit.

		   Generate a new vertex format in case one of the
		   arrays has changed shape since it was cached.  The
		   spec doesn't require us to this (since changing a
		   locked array results in undefined behaviour), but
		   this is being nice.

		   XXX We could generate batches of indices out of the
		   locked array, like we do with normal batched
		   dispatch.
		*/
		psp_log("can't get idxbuf (%d bytes): falling back to slow path\n",
			idxsize * (count + extra));

		__pspgl_ge_vertex_fmt(pspgl_curctx, &vfmt);
		draw_range_elts_fallback(mode, idx_type, indices, count, &vfmt);
		return;
	}
	
	/* FAST: render directly from locked vertices */

	psp_log("using fast path\n");

	hwformat = l->vfmt.hwformat;
	hwformat |= convert_indices(idxbuf, indices, idx_type, l->first, count);

	/* Add an extra index for the final edge */
	if (mode == GL_LINE_LOOP)
		convert_indices(idxbuf + (idxsize * count), indices, idx_type, l->first, 1);

	sendCommandi(CMD_VERTEXTYPE, hwformat);

	vtxbuf = l->cached_array->array;
	l->cached_array->refcount++;

	sendCommandiUncached(CMD_BASE, ((unsigned)vtxbuf >> 8) & 0xf0000);
	sendCommandiUncached(CMD_VERTEXPTR, ((unsigned)vtxbuf) & 0xffffff);
	sendCommandiUncached(CMD_BASE, ((unsigned)idxbuf >> 8) & 0xf0000);
	sendCommandiUncached(CMD_INDEXPTR, ((unsigned)idxbuf) & 0xffffff);

	sendCommandiUncached(CMD_PRIM, (prim << 16) | count);

	__pspgl_dlist_set_cleanup(__pspgl_dlist_cleanup_varray, l->cached_array);
}

/* 
   The hardware has direct support for drawing indexed vertex arrays,
   which we try hard to use.

   There are 4 cases for drawing an indexed array (from best to worst):

   1. Our vertex array is locked, and the index array will fit into
      the command buffer. We don't need to do any format converion, and
      we can directly use hardware indexing.

   2. The array is not locked, but it is small enough to fit into the
      vertex buffer, along with the indices.  Can use hardware indexing.

   3. The array is locked, but the indices are too large to fit in the
      command buffer.  Slow path.

   4. The array is not locked, and the vertex+index data won't fit in
      the command buffer.  Slow path.

   In summary: 1: Whahoo! 2: Yay!  3+4: Boo!
*/
void __pspgl_varray_draw_range_elts(GLenum mode, GLenum idx_type, 
				    const void *indices, GLsizei count, 
				    unsigned minidx, unsigned maxidx)
{
	struct locked_arrays *l = &pspgl_curctx->vertex_array.locked;
	long prim;
	struct vertex_format vfmt;
	struct prim_info pi;
	unsigned maxbatch;
	unsigned numvtx = maxidx - minidx + 1;
	unsigned vtxsize, idxsize;
	unsigned hwformat;
	unsigned extra = 0;

	prim = __pspgl_glprim2geprim(mode);

	if (prim < 0) {
		GLERROR(GL_INVALID_ENUM);
		return;
	}

	if (count == 0)
		return;

	__pspgl_context_flush_pending_matrix_changes(pspgl_curctx);

	if (minidx >= l->first &&
	    maxidx <= (l->first + l->count) &&
	    __pspgl_cache_arrays()) {
		/* We have potentially usable locked+cached arrays */
		draw_range_elts_locked(mode, idx_type, indices, count);
		return;
	}

	psp_log("no locked arrays\n");

	pi = __pspgl_prim_info[prim];
	__pspgl_ge_vertex_fmt(pspgl_curctx, &vfmt);

	if (vfmt.hwformat == 0)
		return;

	vtxsize = ROUNDUP(numvtx * vfmt.vertex_size, 16);
	idxsize = idx_sizeof(idx_type);

	if (idxsize == 0) {
		GLERROR(GL_INVALID_ENUM);
		return;
	}

	/* If this is a line loop, allocate space for an extra index
	   for the final edge */
	extra = 0;
	if (mode == GL_LINE_LOOP)
		extra = 1;

	idxsize *= (count + extra);

	maxbatch = (MAX_VTX_BATCH - idxsize - 16);

	if (maxbatch > 0)
		maxbatch /= vfmt.vertex_size;
	else
		maxbatch = 0;

	hwformat = vfmt.hwformat;

	psp_log("prim=%d maxbatch=%d numvtx=%d count=%d maxidx=%d minidx=%d\n",
		prim, maxbatch, numvtx, count, maxidx, minidx);

	if (numvtx >= maxbatch) {
		/* SLOW: vertex+index data won't fit in command buffer */
		draw_range_elts_fallback(mode, idx_type, indices, count, &vfmt);
		return;
	}


	/* FAST(ish): offload everything in one step */

	unsigned char *buf = __pspgl_dlist_insert_space(pspgl_curctx->dlist_current, 
							vtxsize + idxsize);
	unsigned char *vtxbuf = buf;
	void *idxbuf = buf+vtxsize;
	unsigned done;

	if (buf)
		done = __pspgl_gen_varray(&vfmt, minidx, numvtx, vtxbuf, vtxsize);

	if (!buf || done != numvtx) {
		/* SLOW: couldn't use fast path */
		draw_range_elts_fallback(mode, idx_type, indices, count, &vfmt);
		return;
	}

	hwformat |= convert_indices(idxbuf, indices, idx_type, minidx, count);

	/* Add an extra index for the final edge of a line loop */
	if (mode == GL_LINE_LOOP)
		convert_indices(idxbuf + (idxsize * count), indices, idx_type, minidx, 1);

	sendCommandi(CMD_VERTEXTYPE, hwformat);

	sendCommandiUncached(CMD_BASE, ((unsigned)vtxbuf >> 8) & 0xf0000);
	sendCommandiUncached(CMD_VERTEXPTR, ((unsigned)vtxbuf) & 0xffffff);
	sendCommandiUncached(CMD_BASE, ((unsigned)idxbuf >> 8) & 0xf0000);
	sendCommandiUncached(CMD_INDEXPTR, ((unsigned)idxbuf) & 0xffffff);

	sendCommandiUncached(CMD_PRIM, (prim << 16) | count);
}
