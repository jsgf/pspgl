#include <stdlib.h>
#include "pspgl_internal.h"


//static inline unsigned long align256 (unsigned long x) { return (x + 0xff) & ~0xff; }
/* XXX do we really need 512 byte alignments?!? are 256 enough? */
static inline unsigned long align512 (unsigned long x) { return (x + 0x1ff) & ~0x1ff; }


EGLSurface eglCreateWindowSurface (EGLDisplay dpy, EGLConfig config, NativeWindowType window, const EGLint *attrib_list)
{
	GLint pixelformat = (int) config;
	struct pspgl_surface *s = malloc(sizeof(struct pspgl_surface));
	unsigned long bytesperpixel = (pixelformat == GL_RGBA) ? 4 : 2;
	unsigned long bufferlen;

	if (!s)
		goto bad_alloc;

/* shouldn't this get passed through the attribute list? spec?!? */
int has_frontbuffer = 1;
int has_backbuffer = 1;
int has_depthbuffer = 1;
int has_stencilbuffer = 1;

	s->width = 480;
	s->height = 272;
	s->pixelperline = align512(s->width);
	s->pixelformat = pixelformat;
	s->current_front = 0;
	s->displayed = 1;

	bufferlen = s->height * s->pixelperline * bytesperpixel;

	psp_log("width = %lu\n", s->width);
	psp_log("height = %lu\n", s->height);
	psp_log("pixelperline = %lu\n", s->pixelperline);
	psp_log("pixelformat = 0x%04x\n", s->pixelformat);

	if (has_frontbuffer) {
		if (!(s->color_buffer[0] = pspgl_vidmem_alloc(bufferlen)))
			goto bad_alloc;
	}

	if (has_backbuffer) {
		if (!(s->color_buffer[1] = pspgl_vidmem_alloc(bufferlen)))
			goto bad_alloc;
	}

	bufferlen = s->height * s->pixelperline * 2;

	if (has_depthbuffer) {
		if (!(s->depth_buffer = pspgl_vidmem_alloc(bufferlen)))
			goto bad_alloc;
	}

	return (EGLSurface) s;

bad_alloc:
	eglDestroySurface(dpy, s);
	EGLERROR(EGL_BAD_ALLOC);
	return NULL;
}

