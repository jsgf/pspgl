#include "pspgl_internal.h"


struct clear_vertex {
	unsigned long color;
	GLfloat x;
	GLfloat y;
	GLfloat z;
};


static inline void * align16 (void *ptr) { return (void*) ((((unsigned long) ptr) + 0x0f) & ~0x0f); }

void glClear (GLbitfield mask)
{
	struct pspgl_dlist *dlist = pspgl_curctx->dlist_current;
	struct clear_vertex *vbuf;
	unsigned long cmd = 1;

	/* make room for 2 embedded vertices in cmd_buf, align to 16byte boundary */
	vbuf = align16(pspgl_dlist_insert_space(dlist, 16 + 2 * sizeof(struct clear_vertex)));

	if (!vbuf) {
		GLERROR(GL_OUT_OF_MEMORY);
		return;
	}

	vbuf[0].color = COLOR4(pspgl_curctx->clear.color);
	vbuf[0].x = 0.0;
	vbuf[0].y = 0.0;
	vbuf[0].z = pspgl_curctx->clear.depth;

	vbuf[1].color = vbuf[0].color;
	vbuf[1].x = 480.0;
	vbuf[1].y = 272.0;
	vbuf[1].z = pspgl_curctx->clear.depth;

	if (mask & GL_COLOR_BUFFER_BIT)
		cmd |= (1 << 8);

	if (pspgl_curctx->draw->stencil_buffer && (mask & GL_STENCIL_BUFFER_BIT))
		cmd |= (1 << 9);

	if (pspgl_curctx->draw->depth_buffer && (mask & GL_DEPTH_BUFFER_BIT))
		cmd |= (1 << 10);

	/* enable clear mode */
	sendCommandi(211, cmd);

	/* draw array */
	sendCommandi(18, 0x80019c);              /* xform: 2D, vertex format: RGB8888 (uint32), xyz (float32) */
	sendCommandi(16, (((unsigned long) vbuf) >> 8) & 0xf0000); /* vertex array BASE */
	sendCommandi(1, ((unsigned long) vbuf) & 0xffffff);        /* vertex array, Adress */
	sendCommandi(4, (6 << 16) | 2);          /* sprite (type 6), 2 vertices */

	/* leave clear mode */
	sendCommandi(211, 0);
}
