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
static void flush_pending_state_changes (struct pspgl_context *c)
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
			if ((word & 1) && (c->ge_reg[j] >> 24) == j)
				__pspgl_dlist_enqueue_cmd(c->dlist_current,
							  c->ge_reg[j]);
		}
	}
}


/**
 *  trigger some real action
 */
void __pspgl_context_writereg_uncached (struct pspgl_context *c, unsigned long cmd, unsigned long argi) 
{
	unsigned long val = ((cmd) << 24) | ((argi) & 0xffffff);

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

static void flush_pending_matrix_changes (struct pspgl_context *c)
{
	flush_matrix(c, CMD_MAT_PROJ_TRIGGER, &c->projection_stack);
	flush_matrix(c, CMD_MAT_MODEL_TRIGGER, &c->modelview_stack);
	flush_matrix(c, CMD_MAT_TEXTURE_TRIGGER, &c->texture_stack);
}

/* Do all the pre-render state flushing, and actually emit a primitive */
void __pspgl_context_render_prim(struct pspgl_context *c, 
				 unsigned prim, unsigned count, unsigned vtxfmt,
				 const void *vertex, const void *index)
{
	__pspgl_context_writereg(c, CMD_VERTEXTYPE, vtxfmt);

	if ((vtxfmt & GE_TRANSFORM_SHIFT(1)) == GE_TRANSFORM_3D)
		flush_pending_matrix_changes(c);
	flush_pending_state_changes(c);

	__pspgl_context_writereg_uncached(c, CMD_BASE, ((unsigned)vertex >> 8) & 0x000f0000);
	__pspgl_context_writereg_uncached(c, CMD_VERTEXPTR, ((unsigned)vertex) & 0x00ffffff);
	if (vtxfmt & GE_VINDEX_SHIFT(3)) {
		assert(index != NULL);
		__pspgl_context_writereg_uncached(c, CMD_BASE, ((unsigned)index >> 8) & 0x000f0000);
		__pspgl_context_writereg_uncached(c, CMD_INDEXPTR,  ((unsigned)index) & 0x00ffffff);
	} else
		assert(index == NULL);

	__pspgl_context_writereg_uncached(c, CMD_PRIM, (prim << 16) | count);
}
