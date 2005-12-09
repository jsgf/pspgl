#include <string.h>
#include "pspgl_internal.h"


struct t2f_c4ub_n3f_v3f {
	GLfloat texcoord [2];
	unsigned long color;
	GLfloat normal [3];
	GLfloat vertex [3];
};

#define BUFSZ	12		/* must be a multiple of 2,3 and 4 */

void glVertex3f (GLfloat x, GLfloat y, GLfloat z)
{
	struct pspgl_context *c = pspgl_curctx;
	struct t2f_c4ub_n3f_v3f *vbuf;

	if (c->beginend.vertex_count == 0)
		c->beginend.vbuf_adr = __pspgl_dlist_insert_space(c->dlist_current, BUFSZ * sizeof(struct t2f_c4ub_n3f_v3f));

	vbuf = (struct t2f_c4ub_n3f_v3f *) c->beginend.vbuf_adr;

	if (!vbuf) {
		GLERROR(GL_OUT_OF_MEMORY);
		return;
	}

	vbuf += c->beginend.vertex_count;

	vbuf->texcoord[0] = c->current.texcoord[0];
	vbuf->texcoord[1] = c->current.texcoord[1];
	vbuf->color = c->current.color;
	vbuf->normal[0] = c->current.normal[0];
	vbuf->normal[1] = c->current.normal[1];
	vbuf->normal[2] = c->current.normal[2];
	vbuf->vertex[0] = x;
	vbuf->vertex[1] = y;
	vbuf->vertex[2] = z;

	if (++c->beginend.vertex_count == BUFSZ) {
		static const char overhang [] = { 0, 0, 1, 1, 0, 2, 2, 3, 3, 2 };
		GLenum prim = c->beginend.primitive;

		/* vertex buffer full, render + restart */
		glEnd();

		/* copy overhang */
		c->beginend.vertex_count = overhang[prim];

		if (overhang[prim]) {
			struct t2f_c4ub_n3f_v3f *vbuf_start, *prev;

			prev = c->beginend.vbuf_adr;
			c->beginend.vbuf_adr = __pspgl_dlist_insert_space(c->dlist_current, BUFSZ * sizeof(struct t2f_c4ub_n3f_v3f));
			vbuf_start = c->beginend.vbuf_adr;

			if (prim == GL_TRIANGLE_FAN || prim == GL_POLYGON) {
				memcpy(vbuf_start, prev, sizeof(vbuf_start[0]));
				c->beginend.vertex_count++;
				vbuf_start++;
			}

			memcpy(vbuf_start, vbuf - overhang[prim] + 1, 
			       overhang[prim] * sizeof(vbuf[0]));
		}
 
		/* reset primitive type, was cleared by glEnd() */
		c->beginend.primitive = prim;
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


