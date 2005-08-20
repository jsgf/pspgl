#ifndef __pspgl_dlist_h__
#define __pspgl_dlist_h__

#include "pspgl_misc.h"


#define DLIST_SIZE 1024		/* command words (32bit) */


struct pspgl_dlist {
	struct pspgl_dlist *next;
	struct pspgl_dlist * (*done) (struct pspgl_dlist *thiz);
	unsigned long *cmd_buf;
	unsigned long len;
	int compile_and_run;
	int qid;

	unsigned long __attribute__((aligned(16))) _cmdbuf[DLIST_SIZE + 8];
};


/* Create leading dlist chunk. You can pass done_next_dlist callback = NULL as default. */
extern struct pspgl_dlist* pspgl_dlist_create (int compile_and_run, 
					       struct pspgl_dlist * (*done_next_dlist) (struct pspgl_dlist *thiz));

extern void pspgl_dlist_free (struct pspgl_dlist *d);

/* flush and swap display list buffers in pspgl context. */
extern struct pspgl_dlist* pspgl_dlist_swap (struct pspgl_dlist *thiz);

/* issue an entire dlist list */
extern void pspgl_dlist_submit(struct pspgl_dlist *d);

/* wait until all commands in currently queued dlist are executed and drawn. */
extern void pspgl_dlist_await_completion (void);

/* cancel the currently running wait queue */
extern void pspgl_dlist_cancel (void);

/**
 * insert END/FINISH commands at end of dlist.
 * Update STALL pointer for compile_and_run lists. After this a resubmission is required.
 */
extern void pspgl_dlist_finalize (struct pspgl_dlist *d);

/* prepare dlist for new use */
extern void pspgl_dlist_reset (struct pspgl_dlist *d);

/**
 *  check if dlist has room for (size) bytes embedded in cmd buf and advance cmd_buf pointer,
 *  useful e.g. for vertex data embedded in command stream
 */
extern unsigned long * pspgl_dlist_insert_space (struct pspgl_dlist *d, unsigned long size);


/**
 *  flush commands in dlist, and return a new, reusable dlist
 */
static inline
struct pspgl_dlist * dlist_flush (struct pspgl_dlist *d)
{
	return d->done(d);
}


/**
 *  insert cmd in dlist. If dlist is full, automatically call the done() handler.
 */
extern void pspgl_dlist_enqueue_cmd (struct pspgl_dlist *d, unsigned long cmd);

#endif

