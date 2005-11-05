#include "pspgl_internal.h"
#include "pspgl_texobj.h"

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


static void flush_matrix(struct pspgl_context *c, unsigned opcode, int index,
			 struct pspgl_matrix_stack *stk)
{
	const GLfloat *m;
	int n;
	int i, j;

	if (!(stk->flags & MF_DIRTY))
		return;
	stk->flags &= ~MF_DIRTY;

	if (stk->flags & MF_DISABLED)
		m = __pspgl_identity;
	else
		m = stk->stack[stk->depth].mat;
	n = (opcode == CMD_MAT_PROJ_TRIGGER) ? 4 : 3;

	__pspgl_context_writereg_uncached(c, opcode, index * n * 4);
	opcode++;
	for (j=0; j<4; j++)
		for (i=0; i<n; i++)
			pspgl_context_writereg_mtx(c, opcode, m[4*j+i]);
}

static void flush_pending_matrix_changes (struct pspgl_context *c)
{
	int i;

	flush_matrix(c, CMD_MAT_PROJ_TRIGGER, 0, &c->projection_stack);
	flush_matrix(c, CMD_MAT_MODEL_TRIGGER, 0, &c->modelview_stack);
	flush_matrix(c, CMD_MAT_TEXTURE_TRIGGER, 0, &c->texture_stack);

	for(i = 0; i < NBONES; i++)
		flush_matrix(c, CMD_MAT_BONE_TRIGGER, i, &c->bone_stacks[i]);
}

/* Pin colour map and texture images in memory while there's a pending
   drawing primitive which refers to them */
void __pspgl_context_pin_textures(struct pspgl_context *c)
{
	struct pspgl_texobj *tobj;
	struct pspgl_teximg *cmap;
	int i;

	tobj = c->texture.bound;	

	/* do nothing if there's no texture or texturing is disabled */
	if ((tobj == NULL) ||
	    (c->ge_reg[CMD_ENA_TEXTURE] & 1) == 0)
		return;

	/* find the effective cmap, if any */
	cmap = NULL;
	if (tobj->texfmt)
		cmap = tobj->texfmt->cmap;
	if (cmap == NULL)
		cmap = tobj->cmap;
	if (cmap)
		__pspgl_dlist_pin_buffer(cmap->image);

	/* Walk the images pointed to by the texture object and make
	   sure they're pinned. */
	for (i = 0; i < MIPMAP_LEVELS; i++)
		if (tobj->images[i] && tobj->images[i]->image)
			__pspgl_dlist_pin_buffer(tobj->images[i]->image);
}

void __pspgl_context_render_setup(struct pspgl_context *c, unsigned vtxfmt, 
				  const void *vertex, const void *index)
{
	struct pspgl_texobj *tobj;

	tobj = c->texture.bound;	

	/* set up cmap state; if the texture format has an inherent
	   cmap, use that, otherwise set up the texture object's cmap
	   (if any) */
	if ((tobj != NULL) &&
	    (c->ge_reg[CMD_ENA_TEXTURE] & 1)) {
		struct pspgl_teximg *cmap = NULL;

		if (tobj->texfmt)
			cmap = tobj->texfmt->cmap;
		if (cmap == NULL)
			cmap = tobj->cmap;
		if (cmap) {
			unsigned long p = (unsigned long)cmap->image->base + cmap->offset;

			sendCommandi(CMD_SET_CLUT, p);
			sendCommandi(CMD_SET_CLUT_MSB, (p >> 8) & 0xf0000);
			/* Not sure what the 0xff << 8 is about, but
			   samples/gu/blend.c uses it, and it seems to be
			   necessary to get a non-black output... */
			sendCommandi(CMD_CLUT_MODE, cmap->texfmt->hwformat | (0xff << 8));
			sendCommandi(CMD_CLUT_BLKS, cmap->width / 8);
		}
	}

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
}


/* Do all the pre-render state flushing, and actually emit a primitive */
void __pspgl_context_render_prim(struct pspgl_context *c, 
				 unsigned prim, unsigned count, unsigned vtxfmt,
				 const void *vertex, const void *index)
{
	__pspgl_context_render_setup(c, vtxfmt, vertex, index);

	__pspgl_context_writereg_uncached(c, CMD_PRIM, (prim << 16) | count);

	/* Pin after issuing the CMD_PRIM command, since any command
	   could cause the dlist to be submitted and subsequently
	   unpin everything before the PRIM has actually been issued;
	   it better to unpin too late than too early. */
	__pspgl_context_pin_textures(c);
}
