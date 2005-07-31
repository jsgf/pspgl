#include <stdlib.h>
#include <string.h>
#include <pspge.h>
#include "pspgl_internal.h"

static unsigned long ge_queue_id;


static void pspgl_dlist_jump_next (struct pspgl_dlist *d, struct pspgl_dlist *next);


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


void sendCommandi(unsigned long cmd, unsigned long argi) 
{
	pspgl_dlist_enqueue_cmd(pspgl_curctx->dlist_current, ((cmd) << 24) | ((argi) & 0xffffff));
}


static
void * ptr_align16_uncached (unsigned long ptr)
{
	return (void *) (((ptr + 0x0f) & ~0x0f) | 0x40000000);
}


static
struct pspgl_dlist* pspgl_dlist_finalize_and_clone (struct pspgl_dlist *thiz)
{
	struct pspgl_dlist *next = pspgl_dlist_create(thiz->compile_and_run, NULL);

	if (!next)
		return NULL;

	next->first = thiz->first;
	pspgl_curctx->dlist_current = thiz->next = next;
	pspgl_dlist_jump_next(thiz, next);

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
	struct pspgl_dlist *d = malloc(4 * DLIST_SIZE + sizeof(struct pspgl_dlist) + 0x1f + 2 * 32);

	psp_log("\n");

	if (!d) {
		GLERROR(GL_OUT_OF_MEMORY);
		return NULL;
	}

	d->first = d;
	d->next = NULL;
	d->done = done ? done : pspgl_dlist_finalize_and_clone;
	d->compile_and_run = compile_and_run;
	d->cmd_buf = ptr_align16_uncached((unsigned long) d + 32 + sizeof(struct pspgl_dlist));
	pspgl_dlist_reset(d);

	return d;
}


/**
 *  flush and swap display list buffers in pspgl context. This is a callback only used for internal dlists.
 */
struct pspgl_dlist* pspgl_dlist_swap (struct pspgl_dlist *thiz)
{
	struct pspgl_dlist* next;

	if (pspgl_curctx->dlist_current == pspgl_curctx->dlist[0]) {
		next = pspgl_curctx->dlist[1];
	} else  /* if (pspgl_curctx->dlist_current == pspgl_curctx->dlist[1])*/ {
		next = pspgl_curctx->dlist[0];
	}

	/* wait until backbuffer stall is reached (backbuffer is 'next'). now we can reuse it... */
	sceGeListSync(ge_queue_id, PSP_GE_LIST_STALL_REACHED);

	/* reuse backbuffer and swap */
	pspgl_dlist_jump_next(thiz, next);

	pspgl_curctx->dlist_current = next;

	return pspgl_curctx->dlist_current;
}


void pspgl_dlist_submit (struct pspgl_dlist *d)
{
	void *stall_adr;

	/* find last chunk and determine STALL address */
	while (d->next)
		d = d->next;

	if (d->len == 0)
		d->cmd_buf[d->len++] = 0x00000000;	/* insert STALL point: NOP */

	stall_adr = &d->cmd_buf[d->len-1];

	/* always use head of connected dlists, following chunks are connected by JUMP cmds */
	d = d->first;

	ge_queue_id = sceGeListEnQueue(d->cmd_buf, stall_adr, 0, NULL);
}


void pspgl_dlist_await_completion (void)
{
	sceGeListSync(ge_queue_id, PSP_GE_LIST_DONE);
	sceGeDrawSync(PSP_GE_LIST_DONE);

pspgl_ge_register_dump();
pspgl_ge_matrix_dump();
}


void pspgl_dlist_reset (struct pspgl_dlist *d)
{
	d->cmd_buf[0] = 0x00000000; /* NOP, required as STALL point */
	d->len = 1;
}


void pspgl_dlist_cancel (void)
{
	sceGeListDeQueue(ge_queue_id);
	sceGeListSync(ge_queue_id, PSP_GE_LIST_CANCEL_DONE);
}


void pspgl_dlist_free (struct pspgl_dlist *d)
{
	while (d) {
		struct pspgl_dlist *next = d->next;
		free(d);
		d = next;
	}
}


static
void pspgl_dlist_jump_next (struct pspgl_dlist *d, struct pspgl_dlist *next)
{
	unsigned long adr = (unsigned long) next->cmd_buf;

	if (d->len >= DLIST_SIZE - 2) {
		GLERROR(GL_OUT_OF_MEMORY);
		return;
	}

	pspgl_dlist_reset(next);

	d->cmd_buf[d->len++] = (16 << 24) | ((adr >> 8) & 0xf0000);	/* BASE */
	d->cmd_buf[d->len++] =  (8 << 24) | (adr & 0xffffff);		/* JUMP */

	psp_log("cmd 0x%08x\n", d->cmd_buf[d->len-2]);
	psp_log("cmd 0x%08x\n", d->cmd_buf[d->len-1]);

pspgl_dlist_dump(d->cmd_buf, d->len);

	if (d->compile_and_run)
		sceGeListUpdateStallAddr(ge_queue_id, (void *) adr);
}


void pspgl_dlist_finalize (struct pspgl_dlist *d)
{
	if (d->len >= DLIST_SIZE - 4) {
		GLERROR(GL_OUT_OF_MEMORY);
		return;
	}

	d->cmd_buf[d->len++] = 0x0f000000;	/* FINISH */
	d->cmd_buf[d->len++] = 0x0c000000;	/* END */
	d->cmd_buf[d->len++] = 0x00000000;	/* NOP */
	d->cmd_buf[d->len++] = 0x00000000;

pspgl_dlist_dump(d->cmd_buf, d->len);

	if (d->compile_and_run)
		sceGeListUpdateStallAddr(ge_queue_id, &d->cmd_buf[d->len-1]);
}


unsigned long * pspgl_dlist_insert_space (struct pspgl_dlist *d, unsigned long size)
{
	unsigned long len;
	unsigned long adr;

	size += 0x03 + 2 * sizeof(d->cmd_buf[0]);
	size &= ~0x03;

	if (d->len >= DLIST_SIZE - 4 - size/4) {
		d = d->done(d);
		if (!d || (d->len >= DLIST_SIZE - 4 - size/4))
			return NULL;
	}

	len = d->len;
	d->len += size / sizeof(d->cmd_buf[0]);
	adr = (unsigned long) &d->cmd_buf[d->len];
	d->cmd_buf[len] = (16 << 24) | ((adr >> 8) & 0xf0000);	/* BASE */
	d->cmd_buf[len+1] = (8 << 24) | (adr & 0xffffff);		/* JUMP */

	return (unsigned long*) (&d->cmd_buf[len+2]);
}
