#include <stdlib.h>
#include <string.h>

#include "pspgl_internal.h"
#include "pspgl_buffers.h"

static unsigned pow2(unsigned x)
{
	unsigned ret = 1;

	while(x > ret)
		ret <<= 1;

	return ret;
}

#define MASK(bits)	((1 << (bits)) - 1)

EGLSurface eglCreateWindowSurface (EGLDisplay dpy, EGLConfig config, NativeWindowType window,
				   const EGLint *attrib_list)
{
	struct pspgl_surface *s;
	unsigned long bytesperpixel;
	unsigned long bufferlen;
	int pixidx = EGLCFG_PIXIDX(config);
	const struct pspgl_pixconfig *pixconf = &__pspgl_pixconfigs[pixidx];
	int has_depthbuffer = EGLCFG_HASDEPTH(config);

	s = malloc(sizeof(struct pspgl_surface));
	if (!s)
		goto bad_alloc;
	memset(s, 0, sizeof(*s));

	if (pixconf->hwformat == GE_RGBA_8888)	
		bytesperpixel = 4;
	else
		bytesperpixel = 2;

	/* shouldn't this get passed through the attribute list? spec?!? */
	int has_frontbuffer = 1;
	int has_backbuffer = 1;

	s->width = 480;
	s->height = 272;
	s->pixelperline = pow2(s->width);
	s->current_front = 0;
	s->displayed = 1;
	s->pixfmt = pixconf->hwformat;

	s->alpha_mask	= MASK(pixconf->alpha_bits);
	s->stencil_mask	= MASK(pixconf->stencil_bits);

	bufferlen = s->height * s->pixelperline * bytesperpixel;

	psp_log("width = %lu\n", s->width);
	psp_log("height = %lu\n", s->height);
	psp_log("pixelperline = %lu\n", s->pixelperline);
	psp_log("pixelformat = 0x%04x\n", s->pixfmt);
	psp_log("masks: alpha=%02x stencil=%02x\n",
		s->alpha_mask, s->stencil_mask);

	if (has_frontbuffer) {
		if (!(s->color_buffer[0] = __pspgl_buffer_new(bufferlen, GL_STATIC_COPY_ARB)))
			goto bad_alloc;
		s->color_buffer[0]->flags |= BF_PINNED_FIXED;
	}

	if (has_backbuffer) {
		if (!(s->color_buffer[1] = __pspgl_buffer_new(bufferlen, GL_STATIC_COPY_ARB)))
			goto bad_alloc;
		s->color_buffer[1]->flags |= BF_PINNED_FIXED;
	}

	bufferlen = s->height * s->pixelperline * 2;

	if (has_depthbuffer) {
		if (!(s->depth_buffer = __pspgl_buffer_new(bufferlen, GL_STATIC_COPY_ARB)))
			goto bad_alloc;
		s->depth_buffer->flags |= BF_PINNED_FIXED;
	}

	return (EGLSurface) s;

bad_alloc:
	eglDestroySurface(dpy, s);
	EGLERROR(EGL_BAD_ALLOC);
	return NULL;
}

