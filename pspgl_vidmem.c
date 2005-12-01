#include <stdlib.h>
#include <string.h>
#include <pspdisplay.h>
#include <pspge.h>
#include "pspgl_internal.h"
#include "pspgl_buffers.h"

GLint __pspgl_eglerror = EGL_SUCCESS;
struct pspgl_context *__pspgl_curctx = NULL;


static struct pspgl_buffer **vidmem_map = NULL;
static unsigned long vidmem_map_len = 0, vidmem_map_size = 0;


static int vidmem_map_insert_new (unsigned long idx, struct pspgl_buffer *buf)
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
			return 0;
		vidmem_map = tmp;
	}

	psp_log("alloc vidmem %lu: adr 0x%08p - size 0x%08x\n",
		idx, buf->base, (unsigned int) buf->size);

	memmove(&vidmem_map[idx+1], &vidmem_map[idx],
		(vidmem_map_len-idx) * sizeof(vidmem_map[0]));
	vidmem_map_len++;
	vidmem_map[idx] = buf;

	return 1;
}


int __pspgl_vidmem_alloc (struct pspgl_buffer *buf)
{
	void *start = sceGeEdramGetAddr();
	void *adr = start;
	unsigned long i;
	unsigned size;

	size = ROUNDUP(buf->size, CACHELINE_SIZE); /* make sure eveything is usefully aligned */

	for(i = 0; i < vidmem_map_len; i++) {
		void *new_adr = vidmem_map[i]->base;

		if ((adr+size) <= new_adr) {
			buf->base = adr;
			buf->size = size;
			return vidmem_map_insert_new(i, buf);
		}

		adr = new_adr + vidmem_map[i]->size;
	}

	if ((adr + size) > (start + sceGeEdramGetSize()))
		return 0;

	buf->base = adr;
	buf->size = size;
	return vidmem_map_insert_new(vidmem_map_len, buf);
}

static int addr_cmp(const void *key, const void *b)
{
	const void *k = *(void **)key;
	struct pspgl_buffer **kb = (struct pspgl_buffer **)b;

	if (k < (*kb)->base)
		return -1;
	if (k == (*kb)->base)
		return 0;
	return 1;
}

void  __pspgl_vidmem_free (struct pspgl_buffer *buf)
{
	struct pspgl_buffer **chunk;

	chunk = bsearch(&buf->base, vidmem_map, vidmem_map_len, 
			sizeof(*vidmem_map), addr_cmp);

	if (chunk) {
		int i = chunk - vidmem_map;
		psp_log("free vidmem %d: adr 0x%08p - size 0x%08x\n", i,
			buf->base, buf->size);
			
			vidmem_map_len--;
			memmove(&vidmem_map[i], &vidmem_map[i+1],
				(vidmem_map_len-i) * sizeof(vidmem_map[0]));
	} else
		__pspgl_log("%s: didn't find chunk for pointer %p\n",
			    __FUNCTION__, buf->base);
}


EGLBoolean __pspgl_vidmem_setup_write_and_display_buffer (struct pspgl_surface *s)
{
	int current_back = (s->color_buffer[1] == NULL) ? 0 : (s->current_front ^ 1);
	unsigned long adr;

	s->flip_start = now();

	psp_log("current_front %d\n", s->current_front);

	if (!s) {
		EGLERROR(EGL_BAD_SURFACE);
		return EGL_FALSE;
	}

	psp_log("pixfmt %u\n", s->pixfmt);

	sendCommandi(CMD_PSM, s->pixfmt);

	adr = (unsigned long) s->color_buffer[current_back]->base;
	psp_log("color buffer adr 0x%08x\n", adr);
	sendCommandi(CMD_DRAWBUF, ((adr & 0x00ffffff)));
	sendCommandi(CMD_DRAWBUFWIDTH, ((adr & 0xff000000) >> 8) | s->pixelperline);

	if (s->depth_buffer) {
		adr = (unsigned long) s->depth_buffer->base;
		psp_log("depth buffer adr 0x%08x\n", adr);
		sendCommandi(CMD_DEPTHBUF, ((adr & 0x00ffffff)));
		sendCommandi(CMD_DEPTHBUFWIDTH, ((adr & 0xff000000) >> 8) | s->pixelperline);
	}

	if (s->displayed) {
		/* wait for completion of pending render operations before display */
		__pspgl_buffer_dlist_sync(s->color_buffer[s->current_front]);

		sceDisplaySetFrameBuf(s->color_buffer[s->current_front]->base,
				      s->pixelperline,
				      s->pixfmt,
				      PSP_DISPLAY_SETBUF_NEXTFRAME);

		/* wait for sync if needed */
		if (pspgl_curctx->swap_interval > 0) {
			do {
				sceDisplayWaitVblankStart();
			} while ((sceDisplayGetVcount() % pspgl_curctx->swap_interval) != 0);
		}

		psp_log("display @ adr 0x%08p\n", s->color_buffer[s->current_front]->base);
	}

	s->prev_end = s->flip_end;
	s->flip_end = now();

	return EGL_TRUE;
}

