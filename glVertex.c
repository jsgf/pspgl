#include <string.h>
#include "pspgl_internal.h"


struct t2f_c4ub_n3f_v3f {
	GLfloat texcoord [2];
	unsigned long color;
	GLfloat normal [3];
	GLfloat vertex [3];
};


void glVertex3f (GLfloat x, GLfloat y, GLfloat z)
{
	struct t2f_c4ub_n3f_v3f *vbuf;

	if (pspgl_curctx->current.vertex_count == 0) {
		struct pspgl_dlist *dlist = pspgl_curctx->dlist_current;
		pspgl_curctx->current.vbuf_adr = pspgl_dlist_insert_space(dlist, 12 * sizeof(struct t2f_c4ub_n3f_v3f));
	}

	vbuf = (struct t2f_c4ub_n3f_v3f *) pspgl_curctx->current.vbuf_adr;

	if (!vbuf) {
		GLERROR(GL_OUT_OF_MEMORY);
		return;
	}

	vbuf += pspgl_curctx->current.vertex_count;

	vbuf->texcoord[0] = pspgl_curctx->current.texcoord[0];
	vbuf->texcoord[1] = pspgl_curctx->current.texcoord[1];
	vbuf->color = pspgl_curctx->current.color;
	vbuf->normal[0] = pspgl_curctx->current.normal[0];
	vbuf->normal[1] = pspgl_curctx->current.normal[1];
	vbuf->normal[2] = pspgl_curctx->current.normal[2];
	vbuf->vertex[0] = x;
	vbuf->vertex[1] = y;
	vbuf->vertex[2] = z;

	if (++pspgl_curctx->current.vertex_count == 12) {
		static const char overhang [] = { 0, 0, 1, 1, 0, 2, 2, 3, 3, 2 };
		GLenum prim = pspgl_curctx->current.primitive;

		/* vertex buffer full, render + restart */
		glEnd();

		/* copy overhang */
		pspgl_curctx->current.vertex_count = overhang[prim];

		memcpy(pspgl_curctx->current.vbuf_adr,
		       vbuf - overhang[prim],
		       overhang[prim] * sizeof(*vbuf));

		/* reset primitive type, was cleared by glEnd() */
		pspgl_curctx->current.primitive = prim;
	}
}


void glVertex2f (GLfloat x, GLfloat y)
{
	glVertex3f(x, y, 0.0);
}


void glVertex3fv (const GLfloat *v)
{
	glVertex3f(v[0], v[1], v[2]);
}


void glVertex4fv (const GLfloat *v)
{
	GLfloat scale = 1.0f / v[3];
	glVertex3f(scale * v[0], scale * v[1], scale * v[2]);
}


