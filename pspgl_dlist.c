#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <pspge.h>
#include "pspgl_internal.h"

/* Just for documentation */
#define assert(x)

void pspgl_dlist_enqueue_cmd (struct pspgl_dlist *d, unsigned long cmd)
{
	if (d->len >= DLIST_SIZE - 4) {
		d = dlist_flush(d);
		if (!d)
			return;
	}
	pspgl_curctx->dlist_current->cmd_buf[d->len] = cmd;
	d->len++;
}

static int pspgl_dlist_finish(struct pspgl_dlist *d)
{
	assert(d->len+2 < DLIST_SIZE);

	if (d->len >= DLIST_SIZE - 2) {
		/* XXX should be an assertion */
		GLERROR(GL_OUT_OF_MEMORY);
		return 1;
	}

	d->cmd_buf[d->len++] = 0x0f000000;	/* FINISH */
	d->cmd_buf[d->len++] = 0x0c000000;	/* END */

	return 0;
}

void pspgl_dlist_finalize(struct pspgl_dlist *d)
{
	assert(d->qid == -1);

	if (pspgl_dlist_finish(d))
		return;

	pspgl_dlist_dump(d->cmd_buf, d->len);

	d->qid = sceGeListEnQueue(d->cmd_buf, &d->cmd_buf[d->len], 0, NULL);
}

void sendCommandi(unsigned long cmd, unsigned long argi) 
{
	pspgl_dlist_enqueue_cmd(pspgl_curctx->dlist_current, ((cmd) << 24) | ((argi) & 0xffffff));
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


/**
 *  When allocating the command buffer we need to consider 2 requirements:
 *
 *   - the command buffer start address must be aligned to a 16-byte boundary.
 *   - it must not share any cache line with otherwise used memory.
 *
 *  In order to achieve the 2nd requirement we allocate 32 extra bytes before
 *  and at the end of the command buffer.
 */
struct pspgl_dlist* pspgl_dlist_create (int compile_and_run,
					struct pspgl_dlist * (*done) (struct pspgl_dlist *thiz))
{
	struct pspgl_dlist *d = memalign(16, sizeof(struct pspgl_dlist));

	psp_log("\n");

	if (!d) {
		GLERROR(GL_OUT_OF_MEMORY);
		return NULL;
	}

	d->next = NULL;
	d->done = done ? done : pspgl_dlist_finalize_and_clone;
	d->compile_and_run = compile_and_run;
	d->qid = -1;

	/* use uncached accesses for cmd_buf (place the pointer in high memory) */
	d->cmd_buf = (unsigned long *) (((unsigned long) d->_cmdbuf) | 0x40000000);

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

	size = align16(size + 2 * sizeof(d->cmd_buf[0]));
	size /= sizeof(d->cmd_buf[0]);

	if (d->len >= DLIST_SIZE - 4 - size) {
		d = d->done(d);
		if (!d || (d->len >= DLIST_SIZE - 4 - size))
			return NULL;
	}

	len = d->len;
	d->len += size;
	adr = (unsigned long) &d->cmd_buf[d->len];
	d->cmd_buf[len] = (16 << 24) | ((adr >> 8) & 0xf0000);	/* BASE */
	d->cmd_buf[len+1] = (8 << 24) | (adr & 0xffffff);	/* JUMP */

	return ((void *) align16((unsigned long) &d->cmd_buf[len+2]));
}

