#include "pspgl_internal.h"


/**
 *  cached register write, save value and mark as touched...
 */
void __pspgl_context_writereg (struct pspgl_context *c, unsigned long cmd, unsigned long argi) 
{
	unsigned long new = ((cmd) << 24) | ((argi) & 0xffffff);

	if (new != c->ge_reg[cmd]) {
		c->ge_reg[cmd] = new;
		c->ge_reg_touched[cmd/32] |= (1 << (cmd % 32));
	}
}


void __pspgl_context_writereg_masked (struct pspgl_context *c, unsigned long cmd, unsigned long argi, unsigned long mask)
{
	unsigned long new = (cmd << 24) | (c->ge_reg[cmd] & ~mask) | (argi & mask & 0xffffff);

	if (new != c->ge_reg[cmd]) {
		c->ge_reg[cmd] = new;
		c->ge_reg_touched[cmd/32] |= (1 << (cmd % 32));
	}
}


/**
 *  flush all pending, cached values, then clear register-touch mark words.
 */
void __pspgl_context_flush_pending_state_changes (struct pspgl_context *c)
{
	unsigned i;

	for(i = 0; i < 256; i += 32) {
		uint32_t word = c->ge_reg_touched[i/32];
		unsigned j;

		c->ge_reg_touched[i/32] = 0;

		if (word && 0)
			psp_log("setting i %d word %08x dlist=%p\n",
				i, word, c->dlist_current);

		for(j = i; word != 0; j++, word >>= 1) {
			if (word & 1)
				__pspgl_dlist_enqueue_cmd(c->dlist_current,
							  c->ge_reg[j]);
		}
	}
}


/**
 *  trigger some real action. Implies a flush of pending state changes.
 */
void __pspgl_context_writereg_uncached (struct pspgl_context *c, unsigned long cmd, unsigned long argi) 
{
	unsigned long val = ((cmd) << 24) | ((argi) & 0xffffff);
	__pspgl_context_flush_pending_state_changes(pspgl_curctx);
	__pspgl_dlist_enqueue_cmd(c->dlist_current, val);
}


/* write a uncached matrix register */
static inline
void pspgl_context_writereg_mtx (struct pspgl_context *c, int cmd, GLfloat argf)
{
	union { float f; unsigned int i; } arg = { .f = argf };
	unsigned long val = (cmd << 24) | (arg.i >> 8);
	__pspgl_dlist_enqueue_cmd(c->dlist_current, val);
}


static void flush_matrix(struct pspgl_context *c, unsigned opcode, struct pspgl_matrix_stack *stk)
{
	GLfloat *m;
	int n;
	int i, j;

	if (!stk->dirty)
		return;
	stk->dirty = 0;

	m = stk->stack[stk->depth].mat;
	n = (opcode == CMD_MAT_PROJ_TRIGGER) ? 4 : 3;

	__pspgl_context_writereg_uncached(c, opcode, 0);
	opcode++;
	for (j=0; j<4; j++)
		for (i=0; i<n; i++)
			pspgl_context_writereg_mtx(c, opcode, m[4*j+i]);
}

void __pspgl_context_flush_pending_matrix_changes (struct pspgl_context *c)
{
	flush_matrix(c, CMD_MAT_PROJ_TRIGGER, &c->projection_stack);
	flush_matrix(c, CMD_MAT_MODEL_TRIGGER, &c->modelview_stack);
	flush_matrix(c, CMD_MAT_TEXTURE_TRIGGER, &c->texture_stack);
}
