/*
 * PSP Software Development Kit - http://www.pspdev.org
 * -----------------------------------------------------------------------
 * Licensed under the BSD license, see LICENSE in PSPSDK root for details.
 *
 * pspdisplay.h - Prototypes for the sceDisplay library.
 *
 * Copyright (c) 2005 Marcus R. Brown <mrbrown@ocgnet.org>
 * Copyright (c) 2005 James Forshaw <tyranid@gmail.com>
 * Copyright (c) 2005 John Kelley <ps2dev@kelley.ca>
 *
 * $Id: pspdisplay.h 685 2005-07-18 03:54:59Z ector $
 */
#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#ifdef __cplusplus
extern "C" {
#endif

/** Framebuffer pixel formats. */
enum PspDisplayPixelFormats {
	/** 16-bit RGB 5:6:5. */
	PSP_DISPLAY_PIXEL_FORMAT_565 = 0,
	/** 16-bit RGBA 5:5:5:1. */
	PSP_DISPLAY_PIXEL_FORMAT_5551,
	/* 16-bit RGBA 4:4:4:4. */
	PSP_DISPLAY_PIXEL_FORMAT_4444,
	/* 32-bit RGBA 8:8:8:8. */
	PSP_DISPLAY_PIXEL_FORMAT_8888
};

enum PspDisplaySetBufSync {
	/** Buffer change effective immediately */
	PSP_DISPLAY_SETBUF_IMMEDIATE = 0, 
	/** Buffer change effective next frame */
	PSP_DISPLAY_SETBUF_NEXTFRAME = 1
};

/**
 * Set display mode
 *
 * @par Example1:
 * @code
 * @endcode
 *
 * @param mode - Display mode, normally 0.
 * @param width - Width of screen in pixels.
 * @param height - Height of screen in pixels.
 *
 * @returns ???
 */
int sceDisplaySetMode(int mode, int width, int height);

/**
 * Get display mode
 *
 * @param pmode - Pointer to an integer to receive the current mode.
 * @param pwidth - Pointer to an integer to receive the current width.
 * @param pheight - Pointer to an integer to receive the current height,
 * 
 * @return 0 on success
 */
int sceDisplayGetMode(int *pmode, int *pwidth, int *pheight);

/**
 * Display set framebuf
 *
 * @param topaddr - address of start of framebuffer
 * @param bufferwidth - buffer width (must be power of 2)
 * @param pixelformat - One of ::PspDisplayPixelFormats.
 * @param sync - One of ::PspDisplaySetBufSync
 */
void sceDisplaySetFrameBuf(void *topaddr, int bufferwidth, int pixelformat, int sync);

/**
 * Get Display Framebuffer information
 *
 * @param topaddr - pointer to void* to receive address of start of framebuffer
 * @param bufferwidth - pointer to int to receive buffer width (must be power of 2)
 * @param pixelformat - pointer to int to receive one of ::PspDisplayPixelFormats.
 * @param unk1 - pointer to int, receives unknown, always 1? (vblank sync?)
 */
int sceDisplayGetFrameBuf(void **topaddr, int *bufferwidth, int *pixelformat, int *unk1);

/**
 * Wait for vertical blank
 *
 */
int sceDisplayWaitVblankStart(void);


/* debug log macros, disabled by default. */
#if 0
#define sceDisplaySetMode(mode,w,h) do { psp_log("sceDisplaySetMode(%d, %d, %d)\n", mode, w, h); sceDisplaySetMode(mode, w, h); } while (0)
#define sceDisplaySetFrameBuf(adr,w,fmt,sync) do { psp_log("sceDisplaySetFrameBuf(0x%08x, %d, %d, %d)\n", adr, w, fmt, sync); sceDisplaySetFrameBuf(adr,w,fmt,sync); } while (0)
#define sceDisplayWaitVblankStart() do { psp_log("sceDisplayWaitVblankStart\n"); sceDisplayWaitVblankStart(); } while (0)
#endif

#ifdef __cplusplus
}
#endif

#endif
