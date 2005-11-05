#include <malloc.h>

#include "pspgl_internal.h"
#include "pspgl_buffers.h"

static void bezier_draw_locked(unsigned u, unsigned v, int first)
{
	struct pspgl_context *c = pspgl_curctx;
	struct locked_arrays *l = &c->vertex_array.locked;
	const void *buf;

	first -= l->cached_first;

	buf = l->cached_array->base + l->cached_array_offset;
	buf += first * l->vfmt.vertex_size;

	__pspgl_context_render_setup(c, l->vfmt.hwformat, buf, NULL);
	__pspgl_context_writereg_uncached(c, CMD_BEZIER, (u << 8) | v);
	__pspgl_dlist_pin_buffer(l->cached_array);
	__pspgl_context_pin_textures(c);
}

void glDrawBezierArraysPSP(GLenum mode, GLuint u, GLuint v, GLint first)
{
	struct pspgl_context *c = pspgl_curctx;
	struct vertex_format vfmt;
	GLuint count;
	unsigned size;
	void *buf;
	unsigned prim;

	switch(mode) {
	case GL_TRIANGLES:	prim = 0; break;
	case GL_LINES:		prim = 1; break;
	case GL_POINTS:		prim = 2; break;

	default:
		GLERROR(GL_INVALID_ENUM);
		return;
	}
	sendCommandi(CMD_PATCH_PRIM, prim);

	if (__pspgl_cache_arrays()) {
		bezier_draw_locked(u, v, first);
		return;
	}

	__pspgl_ge_vertex_fmt(c, &vfmt);

	count = u*v;
	size = count * vfmt.vertex_size;

	buf = __pspgl_dlist_insert_space(c->dlist_current, size);

	if (buf == NULL) {
		GLERROR(GL_OUT_OF_MEMORY);
		return;
	}

	if (__pspgl_gen_varray(&vfmt, first, count, buf, size) != count) {
		/* ? */
		GLERROR(GL_OUT_OF_MEMORY);
		return;
	}


	__pspgl_context_render_setup(c, vfmt.hwformat, buf, NULL);
	__pspgl_context_writereg_uncached(c, CMD_BEZIER, (u << 8) | v);
	__pspgl_context_pin_textures(c);
}

void glDrawBezierElementsPSP(GLuint u, GLuint v, GLenum idx_type, const GLvoid *indices)
{
	/* XXX TODO */
}


void glBezierSubdivisionPSP(GLuint u, GLuint v)
{
	sendCommandi(CMD_PATCH_SUBDIV, (u << 8) | v);
}
