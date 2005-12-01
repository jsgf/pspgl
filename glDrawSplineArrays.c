#include <malloc.h>

#include "pspgl_internal.h"
#include "pspgl_buffers.h"

void glDrawSplineArraysPSP(GLenum mode, GLuint u, GLuint v,
			   GLenum u_flags, GLenum v_flags,
			   GLint first)
{
	struct pspgl_context *c = pspgl_curctx;
	struct vertex_format vfmt, *vfmtp;
	struct pspgl_buffer *vbuf;
	unsigned vbuf_offset;
	const void *buf;
	unsigned prim;
	unsigned count;

	if (u_flags < GL_PATCH_INNER_INNER_PSP || u_flags > GL_PATCH_OUTER_OUTER_PSP ||
	    v_flags < GL_PATCH_INNER_INNER_PSP || v_flags > GL_PATCH_OUTER_OUTER_PSP) {
		GLERROR(GL_INVALID_ENUM);
		return;
	}

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

	vbuf = NULL;
	vbuf_offset = 0;

	if (__pspgl_cache_arrays()) {
		/* FAST: draw from locked array */
		struct locked_arrays *l = &c->vertex_array.locked;

		vbuf = l->cached_array;
		vbuf_offset = l->cached_array_offset;
		vfmtp = &l->vfmt;
		first -= l->cached_first;

		vbuf->refcount++;
	}

	if (vbuf == NULL) {
		/* SLOW: convert us some arrays */
		vfmtp = &vfmt;
		__pspgl_ge_vertex_fmt(c, &vfmt);

		if (vfmt.hwformat == 0)
			return;

		vbuf = __pspgl_varray_convert(&vfmt, first, count);

		if (vbuf == NULL) {
			GLERROR(GL_OUT_OF_MEMORY);
			return;
		}
	}

	buf = vbuf->base + vbuf_offset;
	buf += first * vfmtp->vertex_size;

	u_flags &= 3;
	v_flags &= 3;

	__pspgl_context_render_setup(c, vfmtp->hwformat, buf, NULL);
	__pspgl_context_writereg_uncached(c, CMD_SPLINE,
					  (v_flags << 18) | (u_flags << 16) | (v << 8) | u);
	__pspgl_context_pin_buffers(c);
	__pspgl_dlist_pin_buffer(vbuf, BF_PINNED_RD);

	__pspgl_buffer_free(vbuf);
}
