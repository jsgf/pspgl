#include "pspgl_internal.h"


void glEnd (void)
{
	if (pspgl_curctx->current.vertex_count > 0) {
		unsigned long adr = (unsigned long) pspgl_curctx->current.vbuf_adr;
		long prim = __pspgl_glprim2geprim(pspgl_curctx->current.primitive);

		if (prim < 0) {
			GLERROR(GL_INVALID_ENUM);
		} else {
			__pspgl_context_flush_pending_matrix_changes(pspgl_curctx);

			sendCommandi(CMD_VERTEXTYPE, GU_TEXTURE_32BITF | GU_COLOR_8888 | GU_NORMAL_32BITF | GU_VERTEX_32BITF);              /* xform: 3D, vertex format: t2f_c4ub_n3f_v3f */
			sendCommandi(CMD_BASE, (adr >> 8) & 0xf0000); /* vertex array BASE */
			sendCommandi(CMD_VERTEXPTR, adr & 0xffffff);        /* vertex array, Adress */
			sendCommandiUncached(CMD_PRIM, (prim << 16) | pspgl_curctx->current.vertex_count);
		}
	}

	pspgl_curctx->current.primitive = -1;
}
