#include "pspgl_internal.h"


struct clear_vertex {
	unsigned long color;
	GLfloat x;
	GLfloat y;
	GLfloat z;
};


void glClear (GLbitfield mask)
{
	struct pspgl_dlist *dlist = pspgl_curctx->dlist_current;
	struct clear_vertex *vbuf;
	struct pspgl_surface *s = pspgl_curctx->draw;
	unsigned long clearmask = COLOR4(pspgl_curctx->clear.color);
	unsigned long clearmode = 0;

	if (mask & ~(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT)) {
		GLERROR(GL_INVALID_VALUE);
		return;
	}

	/* make room for 2 embedded vertices in cmd_buf, aligned to 16byte boundary */
	vbuf = __pspgl_dlist_insert_space(dlist, 2 * sizeof(struct clear_vertex));

	if (!vbuf) {
		GLERROR(GL_OUT_OF_MEMORY);
		return;
	}

	if (mask & GL_COLOR_BUFFER_BIT) {
		clearmode |= GU_COLOR_BUFFER_BIT;
		if (s->alpha_mask)
			clearmode |= GU_STENCIL_BUFFER_BIT; /* really alpha */
	}

	if (s->stencil_mask && (mask & GL_STENCIL_BUFFER_BIT)) {
		static const unsigned char stencil_shift [] = { 32-1, 32-4, 32-8 };
		clearmask &= 0x00ffffff;
		clearmask |= (pspgl_curctx->clear.stencil) << stencil_shift[s->pixfmt-1];
		clearmode |= GU_STENCIL_BUFFER_BIT;
	}

	if (s->depth_buffer && (mask & GL_DEPTH_BUFFER_BIT))
		clearmode |= GU_DEPTH_BUFFER_BIT;

	vbuf[0].color = clearmask;
	vbuf[0].x = 0.0;
	vbuf[0].y = 0.0;
	vbuf[0].z = pspgl_curctx->clear.depth;

	vbuf[1].color = clearmask;
	vbuf[1].x = 480.0;
	vbuf[1].y = 272.0;
	vbuf[1].z = pspgl_curctx->clear.depth;

	/* enable clear mode */
	sendCommandi(CMD_CLEARMODE, (clearmode << 8) | 1);

	/* draw array */
	sendCommandi(CMD_VERTEXTYPE, GE_COLOR_8888 | GE_VERTEX_32BITF | GE_TRANSFORM_2D);              /* xform: 2D, vertex format: RGB8888 (uint32), xyz (float32) */
	sendCommandiUncached(CMD_BASE, (((unsigned long) vbuf) >> 8) & 0xf0000); /* vertex array BASE */
	sendCommandiUncached(CMD_VERTEXPTR, ((unsigned long) vbuf) & 0xffffff);        /* vertex array, Adress */
	sendCommandiUncached(CMD_PRIM, (GE_SPRITES << 16) | 2);          /* sprite (type 6), 2 vertices */

	/* leave clear mode */
	sendCommandi(CMD_CLEARMODE, 0);
}

