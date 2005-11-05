#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <pspge.h>
#include <psputils.h>

#include "pspgl_internal.h"
#include "pspgl_buffers.h"

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

	if (pspgl_curctx->stats.enabled)
		pspgl_curctx->stats.buffer_issues++;
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

	d = memalign(CACHELINE_SIZE, sizeof(struct pspgl_dlist));

	psp_log("\n");

	if (!d) {
		GLERROR(GL_OUT_OF_MEMORY);
		return NULL;
	}

	d->next = NULL;
	d->done = done ? done : __pspgl_dlist_finalize_and_clone;
	d->compile_and_run = compile_and_run;
	d->qid = -1;

#if !DLIST_CACHED
	d->cmd_buf = __pspgl_uncached(d->_cmdbuf, sizeof(d->_cmdbuf));
#endif

	__pspgl_dlist_reset(d);

	return d;
}

/* Pin a buffer to a particular dlist.  If it is already attached
   to a dlist, move it to this dlist, so it remains pinned a little
   longer. */
void __pspgl_dlist_pin_buffer(struct pspgl_buffer *data)
{
	struct pspgl_dlist *d = pspgl_curctx->dlist_current;

	if (data->pin_prevp != NULL) {
		/* pinned by someone; snip from whatever list its
		   currently on */
		if (data->pin_next)
			data->pin_next->pin_prevp = data->pin_prevp;
		*(data->pin_prevp) = data->pin_next;
	} else {
		/* newly pinned */
		data->pinned++;
		data->refcount++;
	}

	assert(data->pinned > 0);
	assert(data->refcount > data->pinned);

	/* insert into current dlist pin list */
	data->pin_prevp = &d->pins;
	data->pin_next = d->pins;
	if (data->pin_next)
		data->pin_next->pin_prevp = &data->pin_next;
	d->pins = data;
}

static void sync_list(struct pspgl_dlist *list)
{
	unsigned long long start=0;
	struct pspgl_buffer *data, *next;

	if (pspgl_curctx->stats.enabled)
		start = now();

	sceGeListSync(list->qid, PSP_GE_LIST_DONE);

	if (pspgl_curctx->stats.enabled)
		pspgl_curctx->stats.queuewait += __pspgl_ticks_to_us(now() - start);

	list->qid = -1;

	for(data = list->pins; data != NULL; data = next) {
		next = data->pin_next;
		data->pin_prevp = NULL;
		data->pin_next = NULL;

		assert(data->pinned > 0);
		assert(data->refcount >= data->pinned);
		data->pinned--;

		__pspgl_buffer_free(data);
	}
	list->pins = NULL;
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

		if (pspgl_curctx->stats.enabled)
			pspgl_curctx->stats.buffer_issues++;
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
	d->pins = NULL;
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

