#include "pspgl_internal.h"


/**
 *  cached register write, save value and mark as touched...
 */
void pspgl_context_writereg (struct pspgl_context *c, unsigned long cmd, unsigned long argi) 
{
	unsigned long new = ((cmd) << 24) | ((argi) & 0xffffff);

	if (new != c->ge_reg[cmd]) {
		c->ge_reg[cmd] = new;
		c->ge_reg_touched[cmd/32] |= (1 << (31 - cmd % 32));
	}
}


void pspgl_context_writereg_masked (struct pspgl_context *c, unsigned long cmd, unsigned long argi, unsigned long mask)
{
	unsigned long new = (c->ge_reg[cmd] & ~mask) | (argi & mask);

	if (new != c->ge_reg[cmd]) {
		c->ge_reg[cmd] = new;
		c->ge_reg_touched[cmd/32] |= (1 << (31 - cmd % 32));
	}
}


/**
 *  flush all pending, cached values, then clear register-touch mark words.
 */
void pspgl_context_flush_pending_state_changes (struct pspgl_context *c)
{
	unsigned long i;
	struct pspgl_dlist *d = pspgl_curctx->dlist_current;

	for (i=0; i<256/32; i++) {
		register uint32_t word = c->ge_reg_touched[i];
		int idx = 32 * i;

		while (word) {
			register unsigned long count;

			/* count leading zeros */
			__asm__("clz %0, %1" : "=r" (count) : "r" (word));
			word <<= count;
			idx += count;

			/* count leading ones */
			__asm__("clo %0, %1" : "=r" (count) : "r" (word));
			for (; count>0; count--, word<<=1)
				pspgl_dlist_enqueue_cmd(d, c->ge_reg[idx++]);
		}

		c->ge_reg_touched[i] = 0;
	}
}


/**
 *  trigger some real action. Implies a flush of pending state changes.
 */
void pspgl_context_writereg_uncached (struct pspgl_context *c, unsigned long cmd, unsigned long argi) 
{
	unsigned long val = ((cmd) << 24) | ((argi) & 0xffffff);
	pspgl_context_flush_pending_state_changes(pspgl_curctx);
	pspgl_dlist_enqueue_cmd(c->dlist_current, val);
}


/* write a uncached matrix register */
static inline
void pspgl_context_writereg_mtx (struct pspgl_context *c, int cmd, GLfloat argf)
{
	union { float f; unsigned int i; } arg = { .f = argf };
	unsigned long val = (cmd << 24) | (arg.i >> 8);
	pspgl_dlist_enqueue_cmd(c->dlist_current, val);
}


void pspgl_context_flush_pending_matrix_changes (struct pspgl_context *c)
{
	static const unsigned char matrix_opcode [] = { CMD_MAT_MODEL_TRIGGER, CMD_MAT_PROJ_TRIGGER, CMD_MAT_TEXTURE_TRIGGER };
	int matrix_id;

	for (matrix_id=0; matrix_id<3; matrix_id++) {
		if (c->matrix_touched & (1 << matrix_id)) {
			int depth = c->matrix_stack_depth[matrix_id];
			GLfloat *m = c->matrix_stack[matrix_id][depth-1];
			int opcode = matrix_opcode[matrix_id];
			int n = (opcode == CMD_MAT_PROJ_TRIGGER) ? 4 : 3;
			int i, j;

			pspgl_context_writereg_uncached(c, opcode, 0);
			opcode++;

			for (j=0; j<4; j++) {
				for (i=0; i<n; i++) {
					pspgl_context_writereg_mtx(c, opcode, m[4*j+i]);
				}
			}
		}
	}

	c->matrix_touched = 0;
}

