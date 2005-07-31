/*
 * PSP Software Development Kit - http://www.pspdev.org
 * -----------------------------------------------------------------------
 * Licensed under the BSD license, see LICENSE in PSPSDK root for details.
 *
 * pspctrl.h - Prototypes for the sceCtrl library.
 *
 * Copyright (c) 2005 Marcus R. Brown <mrbrown@ocgnet.org>
 * Copyright (c) 2005 James Forshaw <tyranid@gmail.com>
 * Copyright (c) 2005 John Kelley <ps2dev@kelley.ca>
 *
 * $Id: pspctrl.h 698 2005-07-20 06:08:06Z warren $
 */

/* Note: Some of the structures, types, and definitions in this file were
   extrapolated from symbolic debugging information found in the Japanese
   version of Puzzle Bobble. */

#ifndef __CTRL_H__
#define __CTRL_H__

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Ctrl Controller Kernel Library */
/*@{*/

/**
 * Enumeration for the digital controller buttons.
 *
 * @note PSP_CTRL_NOTE can only be read in kernel mode
 */
enum PspCtrlButtons
{
	/** Select button. */
	PSP_CTRL_SELECT     = 0x000001,
	/** Start button. */
	PSP_CTRL_START      = 0x000008,
	/** Up D-Pad button. */
	PSP_CTRL_UP         = 0x000010,
	/** Right D-Pad button. */
	PSP_CTRL_RIGHT      = 0x000020,
	/** Down D-Pad button. */
	PSP_CTRL_DOWN      	= 0x000040,
	/** Left D-Pad button. */
	PSP_CTRL_LEFT      	= 0x000080,
	/** Left trigger. */
	PSP_CTRL_LTRIGGER   = 0x000100,
	/** Right trigger. */
	PSP_CTRL_RTRIGGER   = 0x000200,
	/** Triangle button. */
	PSP_CTRL_TRIANGLE   = 0x001000,
	/** Circle button. */
	PSP_CTRL_CIRCLE     = 0x002000,
	/** Cross button. */
	PSP_CTRL_CROSS      = 0x004000,
	/** Square button. */
	PSP_CTRL_SQUARE     = 0x008000,
	/** Home button. */
	PSP_CTRL_HOME       = 0x010000,
	/** Hold button. */
	PSP_CTRL_HOLD       = 0x020000,
	/** Music Note button. */
	PSP_CTRL_NOTE       = 0x800000,
};

/** Controller mode. */
enum PspCtrlMode
{
	/* Digitial. */
	PSP_CTRL_MODE_DIGITAL = 0,
	/* Analog. */
	PSP_CTRL_MODE_ANALOG
};

/** Returned controller data */
typedef struct SceCtrlData {
	/** The current read frame. */
	unsigned int 	TimeStamp;
	/** Bit mask containing zero or more of ::PspCtrlButtons. */
	unsigned int 	Buttons;
	/** Analogue stick, X axis. */
	unsigned char 	Lx;
	/** Analogue stick, Y axis. */
	unsigned char 	Ly;
	/** Reserved. */
	unsigned char 	Rsrv[6];
} SceCtrlData;

typedef struct SceCtrlLatch {
	unsigned int 	Make;
	unsigned int 	Break;
	unsigned int 	Press;
	unsigned int 	Release;
} SceCtrlLatch;

/**
 * Set the controller cycle setting.
 *
 * @param cycle - Cycle.  Normally set to 0.
 *
 * @returns The previous cycle setting.
 */
int sceCtrlSetSamplingCycle(int cycle);

/**
 * Get the controller current cycle setting.
 *
 * @param pcycle - Return value.
 *
 * @returns 0.
 */
int sceCtrlGetSamplingCycle(int *pcycle);

/**
 * Set the controller mode.
 *
 * @param mode - One of ::PspCtrlMode.
 *
 * @returns The previous mode.
 */
int sceCtrlSetSamplingMode(int mode);

/**
 * Get the current controller mode.
 *
 * @param pmode - Return value.
 *
 * @returns 0.
 */
int sceCtrlGetSamplingMode(int *pmode);

int sceCtrlPeekBufferPositive(SceCtrlData *pad_data, int count);

int sceCtrlPeekBufferNegative(SceCtrlData *pad_data, int count);

/**
 * Read buffer positive
 *
 * @par Example:
 * @code
 * SceCtrlData pad;

 * sceCtrlSetSamplingCycle(0);
 * sceCtrlSetSamplingMode(1);
 * sceCtrlReadBufferPositive(&pad, 1);
 * // Do something with the read controller data
 * @endcode
 *
 * @param pad_data - Pointer to a ::SceCtrlData structure used hold the returned pad data.
 * @param count - Number of ::SceCtrlData buffers to read.
 */
int sceCtrlReadBufferPositive(SceCtrlData *pad_data, int count);

int sceCtrlReadBufferNegative(SceCtrlData *pad_data, int count);

int sceCtrlPeekLatch(SceCtrlLatch *latch_data);

int sceCtrlReadLatch(SceCtrlLatch *latch_data);

/*@}*/

#ifdef __cplusplus
}
#endif

#endif
