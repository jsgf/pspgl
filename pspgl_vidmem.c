#include <stdlib.h>
#include <string.h>
#include <pspdisplay.h>
#include <pspge.h>
#include "pspgl_internal.h"


GLint eglerror = EGL_SUCCESS;
struct pspgl_context *pspgl_curctx = NULL;


struct vidmem_chunk {
	void *ptr;
	unsigned long len;
};


static struct vidmem_chunk *vidmem_map = NULL;
static unsigned long vidmem_map_len = 0;


static
void *vidmem_map_insert_new (unsigned long idx, unsigned long adr, unsigned long size)
{
	void *tmp = realloc(vidmem_map, (vidmem_map_len + 1) * sizeof(vidmem_map[0]));

	if (!tmp)
		return NULL;

	psp_log("alloc vidmem %lu: adr 0x%08x - size 0x%08x\n", idx, (unsigned int) adr, (unsigned int) size);

	vidmem_map = tmp;
	memmove(&vidmem_map[idx+1], &vidmem_map[idx], (vidmem_map_len-idx) * sizeof(vidmem_map[0]));
	vidmem_map_len++;
	vidmem_map[idx].ptr = (void*) adr;
	vidmem_map[idx].len = size;

	return vidmem_map[idx].ptr;
}


void* pspgl_vidmem_alloc (unsigned long size)
{
	unsigned long start = (unsigned long) sceGeEdramGetAddr();
	unsigned long adr = start;
	unsigned long i;

	for (i=0; i<vidmem_map_len; i++) {
		if (vidmem_map[i].ptr != NULL) {
			unsigned long new_adr = (unsigned long) vidmem_map[i].ptr;
			if (size <= new_adr - adr)
				return vidmem_map_insert_new(i, adr, size);
			adr = new_adr + vidmem_map[i].len;
		}
	}

	if (adr + size > start + sceGeEdramGetSize())
		return NULL;

	return vidmem_map_insert_new(vidmem_map_len, adr, size);
}


void  pspgl_vidmem_free (void * ptr)
{
	int i;

	for (i=0; i<vidmem_map_len; i++) {
		if (vidmem_map[i].ptr == ptr) {
			void *tmp;
			
			psp_log("free vidmem %d: adr 0x%08x - size 0x%08x\n", i,
				(unsigned int) ptr, (unsigned int) vidmem_map[i].len);
			
			vidmem_map_len--;
			memmove(&vidmem_map[i], &vidmem_map[i+1], (vidmem_map_len-i) * sizeof(vidmem_map[0]));
			tmp = realloc(vidmem_map, vidmem_map_len * sizeof(vidmem_map[0]));
			if (tmp)
				vidmem_map = tmp;
		}
	}
}


EGLBoolean pspgl_vidmem_setup_write_and_display_buffer (struct pspgl_surface *s)
{
	unsigned long current_back = (s->color_buffer[1] == NULL) ? 0 : (s->current_front ^ 1);
	unsigned long adr;

	psp_log("current_front %d\n", s->current_front);

	if (!s) {
		EGLERROR(EGL_BAD_SURFACE);
		return EGL_FALSE;
	}

	psp_log("pixfmt %u\n", s->pixfmt);

	/* XXX: ?!? how to set up the stencil buffer base ptr ?!? */

	sendCommandi(210, s->pixfmt);

	adr = (unsigned long) s->color_buffer[current_back];
	psp_log("color buffer adr 0x%08x\n", adr);
	sendCommandi(156, ((adr & 0x0000ffffff)));
	sendCommandi(157, ((adr & 0x00ff000000) >> 8) | s->pixelperline);

	if (s->depth_buffer) {
		adr = (unsigned long) s->depth_buffer;
		psp_log("depth buffer adr 0x%08x\n", adr);
		sendCommandi(158, ((adr & 0x0000ffffff)));
		sendCommandi(159, ((adr & 0x00ff000000) >> 8) | s->pixelperline);
	}

	if (s->displayed) {
		/* wait for completion of pending render operations before display */
		glFinish();

		/* wait for sync if needed */
		if (pspgl_curctx->swap_interval > 0) {
			do {
				sceDisplayWaitVblankStart();
			} while ((sceDisplayGetVcount() % pspgl_curctx->swap_interval) != 0);
		}

		psp_log("display @ adr 0x%08x\n", (unsigned long) s->color_buffer[s->current_front]);

		sceDisplaySetFrameBuf(s->color_buffer[s->current_front],
				      s->pixelperline,
				      s->pixfmt,
				      PSP_DISPLAY_SETBUF_NEXTFRAME);
	}

	return EGL_TRUE;
}

