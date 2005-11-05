#include <stdlib.h>
#include "pspgl_internal.h"

static unsigned pow2(unsigned x)
{
	unsigned ret = 1;

	while(x > ret)
		ret <<= 1;

	return ret;
}

#define MASK(bits)	((1 << (bits)) - 1)

EGLSurface eglCreateWindowSurface (EGLDisplay dpy, EGLConfig config, NativeWindowType window, const EGLint *attrib_list)
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

	int red_bits = pixconf->red_bits;
	int green_bits = pixconf->green_bits;
	int blue_bits = pixconf->blue_bits;
	int alpha_bits = pixconf->alpha_bits;
	int stencil_bits = pixconf->stencil_bits;

#if 0
	s->red_mask	= MASK(red_bits);
	s->green_mask	= MASK(green_bits)	<< (red_bits);
	s->blue_mask	= MASK(blue_bits)	<< (red_bits + green_bits);
	/* alpha and stencil are used separately, and don't need shifting */
	s->alpha_mask	= MASK(alpha_bits);
	s->stencil_mask	= MASK(stencil_bits);
#else
	/* It seems that the RGB masks are always 8 bits, even if the
	   buffer format is not RGBA8888 */
	s->red_mask     = red_bits     ? 0x0000ff : 0x000000;
	s->green_mask   = green_bits   ? 0x00ff00 : 0x000000;
	s->blue_mask    = blue_bits    ? 0xff0000 : 0x000000;
	/* alpha and stencil are used separately, and don't need shifting */
	s->alpha_mask	= alpha_bits   ? 0xff : 0x00;
	s->stencil_mask	= stencil_bits ? 0xff : 0x00;
#endif

	bufferlen = s->height * s->pixelperline * bytesperpixel;

	psp_log("width = %lu\n", s->width);
	psp_log("height = %lu\n", s->height);
	psp_log("pixelperline = %lu\n", s->pixelperline);
	psp_log("pixelformat = 0x%04x\n", s->pixfmt);
	psp_log("masks: r=%06x g=%06x b=%06x a=%02x stencil=%02x\n",
		s->red_mask, s->green_mask, s->blue_mask, 
		s->alpha_mask, s->stencil_mask);

	if (has_frontbuffer) {
		if (!(s->color_buffer[0] = __pspgl_vidmem_alloc(bufferlen)))
			goto bad_alloc;
	}

	if (has_backbuffer) {
		if (!(s->color_buffer[1] = __pspgl_vidmem_alloc(bufferlen)))
			goto bad_alloc;
	}

	bufferlen = s->height * s->pixelperline * 2;

	if (has_depthbuffer) {
		if (!(s->depth_buffer = __pspgl_vidmem_alloc(bufferlen)))
			goto bad_alloc;
	}

	return (EGLSurface) s;

bad_alloc:
	eglDestroySurface(dpy, s);
	EGLERROR(EGL_BAD_ALLOC);
	return NULL;
}

