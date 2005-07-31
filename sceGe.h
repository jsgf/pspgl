/*
 * PSP Software Development Kit - http://www.pspdev.org
 * -----------------------------------------------------------------------
 * Licensed under the BSD license, see LICENSE in PSPSDK root for details.
 *
 * pspge.h - Prototypes for the sceGe library.
 *
 * Copyright (c) 2005 Marcus R. Brown <mrbrown@ocgnet.org>
 * Copyright (c) 2005 James Forshaw <tyranid@gmail.com>
 * Copyright (c) 2005 John Kelley <ps2dev@kelley.ca>
 *
 * $Id: pspge.h 550 2005-07-09 05:33:39Z warren $
 */
#ifndef __GE_H__
#define __GE_H__

#include <psptypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Typedef for a GE callback */
typedef void (*GeCallback)(int id, void *arg);

/** Structure to hold the callback data */
typedef struct _GeCallbackData
{
	/** GE callback for the signal interrupt */
	GeCallback signal_func;
	/** GE callback argument for signal interrupt */
	void *signal_arg;
	/** GE callback for the finish interrupt */
	GeCallback finish_func;
	/** GE callback argument for finish interrupt */
	void *finish_arg;
} GeCallbackData;

/**
  * Register callback handlers for the the Ge 
  *
  * @param cb - Configured callback data structure
  * @return The callback ID, < 0 on error
  */
int sceGeSetCallback(GeCallbackData *cb);

/**
  * Unregister the callback handlers
  *
  * @param cbid - The ID of the callbacks from sceGeSetCallback
  * @return < 0 on error
  */
int sceGeUnsetCallback(int cbid);

/** 
  * Save Graphics State in ctx.
  *
  * @param ctx - Storage pointer into 512-byte array
  *
  * @return Unknown. Probably 0 if successful.
  */
int sceGeSaveContext (unsigned long ctx [512]);

/** 
  * Restore Graphics State stored in ctx.
  *
  * @param ctx - Storage pointer into 512-byte array
  *
  * @return Unknown. Probably 0 if successful.
  */
int sceGeRestoreContext (const unsigned long ctx [512]);

/** 
  * Enqueue a display list.
  *
  * @param list - The head of the list to queue.
  * @param stall - The stall address.
  * If NULL then no stall address set and the list is transferred immediately.
  * @param cbid - ID of the callback set by calling sceGeSetCallback
  * @param arg - Probably a parameter to the callbacks (to be confirmed)
  *
  * @return The ID of the queue.
  */
int sceGeListEnQueue (void *list, void *stall, int cbid, void *arg);

/**
  * Cancel a queued or running list
  *
  * @return Unknown. Probably 0 if successful.
  */
int sceGeListDeQueue (int qid);

/**
  * Update the stall address for the specified queue.
  * 
  * @param qid - The ID of the queue.
  * @param stall - The stall address to update
  *
  * @return Unknown. Probably 0 if successful.
  */
int sceGeListUpdateStallAddr(int qid, void *stall);


/**
 *  return the value of command register <cmd>
 */
extern unsigned long sceGeGetCmd (int cmd);


#define GE_MATRIX_BONE0 0
#define GE_MATRIX_BONE1 1
#define GE_MATRIX_BONE2 2
#define GE_MATRIX_BONE3 3
#define GE_MATRIX_BONE4 4
#define GE_MATRIX_BONE5 5
#define GE_MATRIX_BONE6 6
#define GE_MATRIX_BONE7 7
#define GE_MATRIX_WORLD 8
#define GE_MATRIX_VIEW  9
#define GE_MATRIX_PROJ  10
#define GE_MATRIX_TGEN  11

/**
 *  Store the matrix defined by [id] (one of the GE_MATRIX_ enums) in [m].
 */
extern int sceGeGetMtx(int id, float *m);


/** Get the address of VRAM (normally 0x04000000?)
  *
  * @return The base VRAM address
  */
u32 sceGeEdramGetAddr (void);

/** Get the size of VRAM
  *
  * @return The VRAM size
  */
u32 sceGeEdramGetSize (void);


#define GE_LIST_DONE		0
#define GE_LIST_QUEUED		1
#define GE_LIST_DRAWING_DONE	2
#define GE_LIST_STALL_REACHED	3
#define GE_LIST_CANCEL_DONE	4

/** Wait for syncronisation of the list (i.e. the list has ended)
  *
  * @param qid - The queue id of the list
  * @param wait - one of the GE_LIST_XXX defines
  * @return 0?
  */
int sceGeListSync(int qid, int wait);

/**
  * Wait for drawing to complete.
  * 
  * @param wait - one of the GE_LIST_XXX defines
  * @return 0?
  */
int sceGeDrawSync(int wait);


/**
 * debug log macros. Disabled by default.
 */
#if 0
#define sceGeListEnQueue(list,stall,cbid,arg) sceGeListEnQueue(list, stall,cbid,arg); psp_log("sceGeListEnQueue(list %p, stall 0x%08x, %d, %p)\n", list, (unsigned int) stall, cbid, arg);
#define sceGeListDeQueue(qid) do { psp_log("sceGeListDeQueue(qid %d)\n", qid); sceGeListDeQueue(qid); } while (0)
#define sceGeListUpdateStallAddr(qid,stall) do { psp_log("sceGeListUpdateStallAddr(qid %d, stall 0x%08x)\n", qid, (unsigned int) stall); sceGeListUpdateStallAddr(qid, stall); } while (0)
#define sceGeListSync(qid,wait) do { psp_log("sceGeListSync(qid %d, wait %d)\n", qid, wait); sceGeListSync(qid, wait); } while (0)
#define sceGeDrawSync(wait) do { psp_log("sceGeDrawSync(wait %d)\n", wait); sceGeDrawSync(wait); } while (0)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __GE_H__ */
