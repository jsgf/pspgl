#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <pspge.h>
#include "pspgl_internal.h"

/* Just for documentation */
#define assert(x)

void pspgl_dlist_enqueue_cmd (struct pspgl_dlist *d, unsigned long cmd)
{
	if (d->len >= DLIST_SIZE - 2) {
		d = dlist_flush(d);
		if (!d)
			return;
	}
	pspgl_curctx->dlist_current->cmd_buf[d->len] = cmd;
	d->len++;
}


/**
 *  cached register write, save value and mark as touched...
 */
void sendCommandi (unsigned long cmd, unsigned long argi) 
{
	pspgl_curctx->ge_reg[cmd] = ((cmd) << 24) | ((argi) & 0xffffff);
	pspgl_curctx->ge_reg_touched[cmd/32] |= (1 << (cmd % 32));
}


/* write a uncached matrix register */
static inline
void sendCommandfMtx (int cmd, GLfloat argf)
{
	union { float f; unsigned int i; } arg = { .f = argf };
	unsigned long val = (cmd << 24) | (arg.i >> 8);
	pspgl_dlist_enqueue_cmd(pspgl_curctx->dlist_current, val);
}


void pspgl_flush_pending_matrix_changes (struct pspgl_context *c)
{
	static const unsigned char matrix_opcode [] = { 58, 62, 64 };
	int matrix_id;

	for (matrix_id=0; matrix_id<3; matrix_id++) {
		if (c->matrix_touched & (1 << matrix_id)) {
			int depth = c->matrix_stack_depth[matrix_id];
			GLfloat *m = c->matrix_stack[matrix_id][depth-1];
			int opcode = matrix_opcode[matrix_id];
			int n = (opcode == 62) ? 4 : 3;
			int i, j;

			sendCommandiUncached(opcode, 0);
			opcode++;

			for (j=0; j<4; j++) {
				for (i=0; i<n; i++) {
					sendCommandfMtx(opcode, m[4*j+i]);
				}
			}
		}
	}

	c->matrix_touched = 0;
}


/**
 *  flush all pending, cached values, then clear register-touch mark words.
 */
void pspgl_flush_pending_state_changes (struct pspgl_context *c)
{
	unsigned long i;
	struct pspgl_dlist *d = pspgl_curctx->dlist_current;

	for (i=0; i<256/32; i++) {
		uint32_t word = c->ge_reg_touched[i];
		uint32_t idx = 32 * i;

		/**
		 * try to accelerate search for the common case of sparse arrays
		 * by skipping blocks of long zero bit clusters
		 */
		if ((word & 0xffff) == 0) {
			word >>= 16;
			idx += 16;
		}

		while (word) {
			while (word & 1) {
				pspgl_dlist_enqueue_cmd(d, c->ge_reg[idx]);
				word >>= 1;
				idx++;
			}
			if (word == 0)
				break;
			if ((word & 0xff) == 0) {
				word >>= 8;
				idx += 8;
			}
			if ((word & 0xf) == 0) {
				word >>= 4;
				idx += 4;
			}
			if ((word & 0x3) == 0) {
				word >>= 2;
				idx += 2;
			}
			if ((word & 0x1) == 0) {
				word >>= 1;
				idx++;
			}
		}

		c->ge_reg_touched[i] = 0x00000000;
	}
}


/**
 *  trigger some real action. Implies a flush of pending state changes.
 */
void sendCommandiUncached (unsigned long cmd, unsigned long argi) 
{
	unsigned long val = ((cmd) << 24) | ((argi) & 0xffffff);
	pspgl_flush_pending_state_changes(pspgl_curctx);
	pspgl_dlist_enqueue_cmd(pspgl_curctx->dlist_current, val);
}


static
void pspgl_dlist_finish (struct pspgl_dlist *d)
{
	pspgl_flush_pending_state_changes(pspgl_curctx);

	assert(d->len < DLIST_SIZE - 2);

	d->cmd_buf[d->len++] = 0x0f000000;	/* FINISH */
	d->cmd_buf[d->len++] = 0x0c000000;	/* END */
}


void pspgl_dlist_finalize(struct pspgl_dlist *d)
{
	pspgl_dlist_finish(d);
	pspgl_dlist_dump(d->cmd_buf, d->len);
	assert(d->qid == -1);
	d->qid = sceGeListEnQueue(d->cmd_buf, &d->cmd_buf[d->len], 0, NULL);
}


static
struct pspgl_dlist* pspgl_dlist_finalize_and_clone (struct pspgl_dlist *thiz)
{
	struct pspgl_dlist *next = pspgl_dlist_create(thiz->compile_and_run, NULL);

	if (!next)
		return NULL;

	pspgl_dlist_finish(thiz);
	pspgl_curctx->dlist_current = thiz->next = next;

	return next;
}


static inline unsigned long align64 (unsigned long adr) { return (((adr + 0x3f) & ~0x3f) | 0x40000000); }


/**
 *  When allocating the command buffer we need to consider 2 requirements:
 *
 *   - the command buffer start address must be aligned to a 16-byte boundary.
 *   - it must not share any cache line with otherwise used memory. Cache lines are 64bytes long.
 *
 *  In order to achieve the 2nd requirement we allocate 64 extra bytes before
 *  and at the end of the command buffer.
 */
struct pspgl_dlist* pspgl_dlist_create (int compile_and_run,
					struct pspgl_dlist * (*done) (struct pspgl_dlist *thiz))
{
	struct pspgl_dlist *d = malloc(sizeof(struct pspgl_dlist));

	psp_log("\n");

	if (!d) {
		GLERROR(GL_OUT_OF_MEMORY);
		return NULL;
	}

	d->next = NULL;
	d->done = done ? done : pspgl_dlist_finalize_and_clone;
	d->compile_and_run = compile_and_run;
	d->qid = -1;

	d->cmd_buf = (void *) align64((unsigned long) d->_cmdbuf);

	pspgl_dlist_reset(d);

	return d;
}


/**
 *  flush and swap display list buffers in pspgl context. This is a callback only used for internal dlists.
 */
struct pspgl_dlist* pspgl_dlist_swap (struct pspgl_dlist *thiz)
{
	struct pspgl_dlist* next;

	assert(thiz == pspgl->curctx->dlist[pspgl->curctx->dlist_idx])

	pspgl_dlist_finalize(thiz);

	if (++pspgl_curctx->dlist_idx >= NUM_CMDLISTS)
		pspgl_curctx->dlist_idx = 0;
	next = pspgl_curctx->dlist[pspgl_curctx->dlist_idx];

	/* wait until next is done */
	if (next->qid != -1) {
		sceGeListSync(next->qid, PSP_GE_LIST_DONE);
		next->qid = -1;
	}
	pspgl_dlist_reset(next);

	pspgl_curctx->dlist_current = next;

	return next;
}


void pspgl_dlist_submit(struct pspgl_dlist *d)
{
	for(; d != NULL; d = d->next) {
		if (d->qid != -1)
			sceGeListSync(d->qid, PSP_GE_LIST_DONE);
		d->qid = sceGeListEnQueue(d->cmd_buf, &d->cmd_buf[d->len], 0, NULL);
	}
}


void pspgl_dlist_await_completion (void)
{
	int i;

	for(i = 0; i < NUM_CMDLISTS; i++) {
		struct pspgl_dlist *d = pspgl_curctx->dlist[i];

		if (d->qid != -1) {
			sceGeListSync(d->qid, PSP_GE_LIST_DONE);
			d->qid = -1;
			pspgl_dlist_reset(d);
		}
	}

	sceGeDrawSync(PSP_GE_LIST_DONE);

	pspgl_ge_register_dump();
	pspgl_ge_matrix_dump();
}


void pspgl_dlist_reset (struct pspgl_dlist *d)
{
	assert(d->qid == -1);
	d->len = 0;
}


void pspgl_dlist_cancel (void)
{
	int i;

	for(i = 0; i < NUM_CMDLISTS; i++) {
		struct pspgl_dlist *d = pspgl_curctx->dlist[i];

		if (d->qid != -1) {
			sceGeListDeQueue(d->qid);
			sceGeListSync(d->qid, PSP_GE_LIST_CANCEL_DONE);
		}
	}
}


void pspgl_dlist_free (struct pspgl_dlist *d)
{
	while (d) {
		struct pspgl_dlist *next = d->next;
		assert(d->qid == -1);
		free(d);
		d = next;
	}
}


static inline unsigned long align16 (unsigned long val) { return ((((unsigned long) val) + 0x0f) & ~0x0f); }

void * pspgl_dlist_insert_space (struct pspgl_dlist *d, unsigned long size)
{
	unsigned long len;
	unsigned long adr;

	size = align16(size + 0x0f + 2 * sizeof(d->cmd_buf[0]));
	size /= sizeof(d->cmd_buf[0]);

	pspgl_flush_pending_state_changes(pspgl_curctx);

	if (d->len >= DLIST_SIZE - 2 - size) {
		d = d->done(d);
		if (!d || (d->len >= DLIST_SIZE - 2 - size))
			return NULL;
	}

	len = d->len;
	d->len += size;
	adr = (unsigned long) &d->cmd_buf[d->len];
	d->cmd_buf[len] = (16 << 24) | ((adr >> 8) & 0xf0000);	/* BASE */
	d->cmd_buf[len+1] = (8 << 24) | (adr & 0xffffff);	/* JUMP */

	return ((void *) align16((unsigned long) &d->cmd_buf[len+2]));
}

