#include <stdlib.h>
#include <string.h>
#include <pspdisplay.h>
#include <pspge.h>
#include "pspgl_internal.h"


GLint __pspgl_eglerror = EGL_SUCCESS;
struct pspgl_context *__pspgl_curctx = NULL;


struct vidmem_chunk {
	void *ptr;
	unsigned long len;
};


static struct vidmem_chunk *vidmem_map = NULL;
static unsigned long vidmem_map_len = 0, vidmem_map_size = 0;


static
void *vidmem_map_insert_new (unsigned long idx, void *adr,
			     unsigned long size)
{
	/* grow map if necessary */
	if ((vidmem_map_len+1) > vidmem_map_size) {
		void *tmp;

		if (vidmem_map_size == 0)
			vidmem_map_size = 4;
		else
			vidmem_map_size *= 2;

		tmp = realloc(vidmem_map, vidmem_map_size * sizeof(vidmem_map[0]));

		if (!tmp)
			return NULL;
		vidmem_map = tmp;
	}

	psp_log("alloc vidmem %lu: adr 0x%08p - size 0x%08x\n",
		idx, adr, (unsigned int) size);

	memmove(&vidmem_map[idx+1], &vidmem_map[idx],
		(vidmem_map_len-idx) * sizeof(vidmem_map[0]));
	vidmem_map_len++;
	vidmem_map[idx].ptr = adr;
	vidmem_map[idx].len = size;

	return vidmem_map[idx].ptr;
}


void* __pspgl_vidmem_alloc (unsigned long size)
{
	void *start = sceGeEdramGetAddr();
	void *adr = start;
	unsigned long i;

	size = ROUNDUP(size, CACHELINE_SIZE); /* make sure eveything is usefully aligned */

	for(i = 0; i < vidmem_map_len; i++) {
		void *new_adr = vidmem_map[i].ptr;

		if ((adr+size) <= new_adr)
			return vidmem_map_insert_new(i, adr, size);

		adr = new_adr + vidmem_map[i].len;
	}

	if ((adr + size) > (start + sceGeEdramGetSize()))
		return NULL;

	return vidmem_map_insert_new(vidmem_map_len, adr, size);
}

static int addr_cmp(const void *key, const void *b)
{
	const void *k = *(void **)key;
	const struct vidmem_chunk *kb = b;

	if (k < kb->ptr)
		return -1;
	if (k == kb->ptr)
		return 0;
	return 1;
}

void  __pspgl_vidmem_free (void * ptr)
{
	struct vidmem_chunk *chunk;

	chunk = bsearch(&ptr, vidmem_map, vidmem_map_len, 
			sizeof(*vidmem_map), addr_cmp);

	if (chunk) {
		int i = chunk - vidmem_map;
		psp_log("free vidmem %d: adr 0x%08p - size 0x%08x\n", i,
			ptr, (unsigned int) vidmem_map[i].len);
			
			vidmem_map_len--;
			memmove(&vidmem_map[i], &vidmem_map[i+1],
				(vidmem_map_len-i) * sizeof(vidmem_map[0]));
	} else
		__pspgl_log("%s: didn't find chunk for pointer %p\n",
			    __FUNCTION__, ptr);
}


EGLBoolean __pspgl_vidmem_setup_write_and_display_buffer (struct pspgl_surface *s)
{
	unsigned long current_back = (s->color_buffer[1] == NULL) ? 0 : (s->current_front ^ 1);
	unsigned long adr;

	s->flip_start = now();

	psp_log("current_front %d\n", s->current_front);

	if (!s) {
		EGLERROR(EGL_BAD_SURFACE);
		return EGL_FALSE;
	}

	psp_log("pixfmt %u\n", s->pixfmt);

	/* XXX: ?!? how to set up the stencil buffer base ptr ?!? */

	sendCommandi(CMD_PSM, s->pixfmt);

	adr = (unsigned long) s->color_buffer[current_back];
	psp_log("color buffer adr 0x%08x\n", adr);
	sendCommandi(CMD_DRAWBUF, ((adr & 0x00ffffff)));
	sendCommandi(CMD_DRAWBUFWIDTH, ((adr & 0xff000000) >> 8) | s->pixelperline);

	if (s->depth_buffer) {
		adr = (unsigned long) s->depth_buffer;
		psp_log("depth buffer adr 0x%08x\n", adr);
		sendCommandi(CMD_DEPTHBUF, ((adr & 0x00ffffff)));
		sendCommandi(CMD_DEPTHBUFWIDTH, ((adr & 0xff000000) >> 8) | s->pixelperline);
	}

	if (s->displayed) {
		/* wait for completion of pending render operations before display */
		glFinish();

		sceDisplaySetFrameBuf(s->color_buffer[s->current_front],
				      s->pixelperline,
				      s->pixfmt,
				      PSP_DISPLAY_SETBUF_NEXTFRAME);

		/* wait for sync if needed */
		if (pspgl_curctx->swap_interval > 0) {
			do {
				sceDisplayWaitVblankStart();
			} while ((sceDisplayGetVcount() % pspgl_curctx->swap_interval) != 0);
		}

		psp_log("display @ adr 0x%08x\n", (unsigned long) s->color_buffer[s->current_front]);
	}

	s->prev_end = s->flip_end;
	s->flip_end = now();

	return EGL_TRUE;
}

