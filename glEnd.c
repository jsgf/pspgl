#include "pspgl_internal.h"


void glEnd (void)
{
	if (pspgl_curctx->current.vertex_count > 0) {
		unsigned long adr = (unsigned long) pspgl_curctx->current.vbuf_adr;
		long prim = glprim2geprim(pspgl_curctx->current.primitive);

		if (prim < 0) {
			GLERROR(GL_INVALID_ENUM);
		} else {
			pspgl_context_flush_pending_matrix_changes(pspgl_curctx);
			sendCommandi(18, 0x0001ff);              /* xform: 3D, vertex format: t2f_c4ub_n3f_v3f */
			sendCommandi(16, (adr >> 8) & 0xf0000); /* vertex array BASE */
			sendCommandi(1, adr & 0xffffff);        /* vertex array, Adress */
			sendCommandiUncached(4, (prim << 16) | pspgl_curctx->current.vertex_count);
		}
	}

	pspgl_curctx->current.primitive = -1;
}
