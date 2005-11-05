#include "pspgl_internal.h"

void __pspgl_varray_draw_elts(GLenum mode, GLenum idx_type, const void *indices, GLsizei count)
{
	struct locked_arrays *l = &pspgl_curctx->vertex_array.locked;
	unsigned minidx, maxidx;

	if (l->count > 0) {
		/* use locked range */

		minidx = l->first;
		maxidx = l->first + l->count;
	} else {
		/* find the min and max idx */
		int i;

		minidx = ~0;
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
	}

	__pspgl_varray_draw_range_elts(mode, idx_type, indices, count, minidx, maxidx);
}
