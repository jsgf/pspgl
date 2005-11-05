#include "pspgl_internal.h"

void __pspgl_varray_draw_elts(GLenum mode, GLenum idx_type, const void *indices, GLsizei count)
{
	unsigned minidx, maxidx;
	int i;

	/* XXX if there's a locked array, use its min and max */

	minidx = ~0;
	maxidx = 0;

	/* find the min and max idx */
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

	__pspgl_varray_draw_range_elts(mode, idx_type, indices, count, minidx, maxidx);
}
