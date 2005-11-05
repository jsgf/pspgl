#include "pspgl_internal.h"
#include "pspgl_buffers.h"

void __pspgl_varray_draw_elts(GLenum mode, GLenum idx_type, const void *const indices, GLsizei count)
{
	struct locked_arrays *l = &pspgl_curctx->vertex_array.locked;
	unsigned minidx, maxidx;

	if (l->count > 0) {
		/* use locked range */

		minidx = l->first;
		maxidx = l->first + l->count;
	} else {
		/* find the min and max idx */
		struct pspgl_bufferobj *idxbuf;
		int i;
		const void *idxmap;

		minidx = ~0;
		maxidx = 0;

		idxbuf = pspgl_curctx->vertex_array.indexbuffer;

		if (idxbuf && idxbuf->mapped) {
			GLERROR(GL_INVALID_OPERATION);
			return;
		}

		idxmap = __pspgl_bufferobj_map(idxbuf, GL_READ_ONLY_ARB, (void *)indices);

		switch(idx_type) {
		case GL_UNSIGNED_BYTE:
			for(i = 0; i < count; i++) {
				GLuint idx = ((GLubyte *)idxmap)[i];
				if (idx < minidx)
					minidx = idx;
				if (idx > maxidx)
					maxidx = idx;
			}
			break;

		case GL_UNSIGNED_SHORT:
			for(i = 0; i < count; i++) {
				GLuint idx = ((GLushort *)idxmap)[i];
				if (idx < minidx)
					minidx = idx;
				if (idx > maxidx)
					maxidx = idx;
			}
			break;

		case GL_UNSIGNED_INT:
			for(i = 0; i < count; i++) {
				GLuint idx = ((GLuint *)idxmap)[i];
				if (idx < minidx)
					minidx = idx;
				if (idx > maxidx)
					maxidx = idx;
			}
			break;
		}

		__pspgl_bufferobj_unmap(idxbuf, GL_READ_ONLY_ARB);
	}

	__pspgl_varray_draw_range_elts(mode, idx_type, indices, count, minidx, maxidx);
}
