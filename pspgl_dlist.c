#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <pspge.h>
#include <psputils.h>
#include "pspgl_internal.h"

void __pspgl_dlist_enqueue_cmd (struct pspgl_dlist *d, unsigned long cmd)
{
	if ((d->len + 1) >= (DLIST_SIZE - 4)) {
		d = dlist_flush(d);
		if (!d)
			return;
	}
	d->cmd_buf[d->len++] = cmd;
}


static
void pspgl_dlist_finish (struct pspgl_dlist *d)
{
	if ((d->len + 4) > DLIST_SIZE)
		__pspgl_log("d->len=%d DLIST_SIZE=%d\n", d->len, DLIST_SIZE);
	assert((d->len + 4) <= DLIST_SIZE);

	d->cmd_buf[d->len++] = 0x0f000000;	/* FINISH */
	d->cmd_buf[d->len++] = 0x0c000000;	/* END */
	d->cmd_buf[d->len++] = 0x00000000;
	d->cmd_buf[d->len++] = 0x00000000;
}


void __pspgl_dlist_finalize(struct pspgl_dlist *d)
{
	pspgl_dlist_finish(d);
	pspgl_dlist_dump(d->cmd_buf, d->len);
	assert(d->qid == -1);
	if (DLIST_CACHED)
		sceKernelDcacheWritebackInvalidateRange(d->cmd_buf, sizeof(d->cmd_buf));
	d->qid = sceGeListEnQueue(d->cmd_buf, &d->cmd_buf[d->len-1], 0, NULL);
}


static
struct pspgl_dlist* __pspgl_dlist_finalize_and_clone (struct pspgl_dlist *thiz)
{
	struct pspgl_dlist *next = __pspgl_dlist_create(thiz->compile_and_run, NULL);

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
struct pspgl_dlist* __pspgl_dlist_create (int compile_and_run,
				     struct pspgl_dlist * (*done) (struct pspgl_dlist *thiz))
{
	struct pspgl_dlist *d;

	if (DLIST_CACHED)
		d = memalign(64, sizeof(struct pspgl_dlist));
	else
		d = malloc(sizeof(struct pspgl_dlist));

	psp_log("\n");

	if (!d) {
		GLERROR(GL_OUT_OF_MEMORY);
		return NULL;
	}

	d->next = NULL;
	d->done = done ? done : __pspgl_dlist_finalize_and_clone;
	d->compile_and_run = compile_and_run;
	d->qid = -1;
	d->cleanups = NULL;

#if !DLIST_CACHED
	sceKernelDcacheWritebackInvalidateRange(d->_cmdbuf, sizeof(d->_cmdbuf));
	d->cmd_buf = (void *) align64((unsigned long) d->_cmdbuf);
#endif

	__pspgl_dlist_reset(d);

	return d;
}

struct pspgl_cleanup {
	void (*cleanup)(void *);
	void *cleanup_arg;

	struct pspgl_cleanup *next;
};

void __pspgl_dlist_set_cleanup(void (*cleanup)(void *), void *arg)
{
	struct pspgl_dlist *list = pspgl_curctx->dlist[pspgl_curctx->dlist_idx];
	struct pspgl_cleanup *c;

	c = malloc(sizeof(*c));

	if (c == NULL) {
		/* XXX What to do?  Call cleanup now? */
		psp_log("can't allocate cleanup for %p/%p\n", cleanup, arg);
		return;
	}

	c->cleanup = cleanup;
	c->cleanup_arg = arg;
	c->next = list->cleanups;
	list->cleanups = c;
}

static void sync_list(struct pspgl_dlist *list)
{
	struct pspgl_cleanup *cleanup, *next;
	sceGeListSync(list->qid, PSP_GE_LIST_DONE);
	list->qid = -1;

	for(cleanup = list->cleanups; cleanup != NULL; cleanup = next) {
		next = cleanup->next;

		(*cleanup->cleanup)(cleanup->cleanup_arg);
		free(cleanup);
	}
	list->cleanups = NULL;
}

/**
 *  flush and swap display list buffers in pspgl context. This is a callback only used for internal dlists.
 */
struct pspgl_dlist* __pspgl_dlist_swap (struct pspgl_dlist *thiz)
{
	struct pspgl_dlist* next;

	assert(thiz == pspgl_curctx->dlist[pspgl_curctx->dlist_idx]);

	__pspgl_dlist_finalize(thiz);

	if (++pspgl_curctx->dlist_idx >= NUM_CMDLISTS)
		pspgl_curctx->dlist_idx = 0;

	next = pspgl_curctx->dlist[pspgl_curctx->dlist_idx];

	/* wait until next is done */
	if (next->qid != -1)
		sync_list(next);
	__pspgl_dlist_reset(next);

	pspgl_curctx->dlist_current = next;

	return next;
}


void __pspgl_dlist_submit(struct pspgl_dlist *d)
{
	for(; d != NULL; d = d->next) {
		if (d->qid != -1)
			sync_list(d);
		if (DLIST_CACHED)
			sceKernelDcacheWritebackInvalidateRange(d->cmd_buf, sizeof(d->cmd_buf));
		d->qid = sceGeListEnQueue(d->cmd_buf, &d->cmd_buf[d->len], 0, NULL);
	}
}


void __pspgl_dlist_await_completion (void)
{
	int i;

	for(i = 0; i < NUM_CMDLISTS; i++) {
		struct pspgl_dlist *d = pspgl_curctx->dlist[i];

		if (d->qid != -1) {
			sync_list(d);
			__pspgl_dlist_reset(d);
		}
	}

	sceGeDrawSync(PSP_GE_LIST_DONE);

	pspgl_ge_register_dump();
	pspgl_ge_matrix_dump();
}


void __pspgl_dlist_reset (struct pspgl_dlist *d)
{
	assert(d->qid == -1);
	d->len = 0;
}


void __pspgl_dlist_cancel (void)
{
	int i;

	for(i = 0; i < NUM_CMDLISTS; i++) {
		struct pspgl_dlist *d = pspgl_curctx->dlist[i];

		if (d->qid != -1) {
			sceGeListDeQueue(d->qid);
			sync_list(d);
		}
	}
}


void __pspgl_dlist_free (struct pspgl_dlist *d)
{
	while (d) {
		struct pspgl_dlist *next = d->next;
		assert(d->qid == -1);
		free(d);
		d = next;
	}
}


static inline unsigned long align16 (unsigned long val) { return ((((unsigned long) val) + 0x0f) & ~0x0f); }

void * __pspgl_dlist_insert_space (struct pspgl_dlist *d, unsigned long size)
{
	unsigned long len;
	unsigned long adr;

	size = align16(size + 0x0f + 2 * sizeof(d->cmd_buf[0]));
	size /= sizeof(d->cmd_buf[0]);

	if ((d->len + size) >= (DLIST_SIZE - 4)) {
		d = d->done(d);
		if (!d || ((d->len + size) >= (DLIST_SIZE - 4)))
			return NULL;
	}

	len = d->len;
	d->len += size;
	adr = (unsigned long) &d->cmd_buf[d->len];
	d->cmd_buf[len] = (16 << 24) | ((adr >> 8) & 0xf0000);	/* BASE */
	d->cmd_buf[len+1] = (8 << 24) | (adr & 0xffffff);	/* JUMP */

	return ((void *) align16((unsigned long) &d->cmd_buf[len+2]));
}

