#include <stdlib.h>
#include "pspgl_internal.h"


//static inline unsigned long align256 (unsigned long x) { return (x + 0xff) & ~0xff; }
/* XXX do we really need 512 byte alignments?!? are 256 enough? */
static inline unsigned long align512 (unsigned long x) { return (x + 0x1ff) & ~0x1ff; }


/* PSP pixelformats */
#define GU_PSM_5650		(0) /* Display, Texture, Palette */
#define GU_PSM_5551		(1) /* Display, Texture, Palette */
#define GU_PSM_4444		(2) /* Display, Texture, Palette */
#define GU_PSM_8888		(3) /* Display, Texture, Palette */
#define GU_PSM_T4		(4) /* Texture */
#define GU_PSM_T8		(5) /* Texture */


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

	s->width = 480;
	s->height = 272;
	s->pixelperline = align512(s->width);
	s->current_front = 0;
	s->displayed = 1;

	psp_log("GL pixelformat = 0x%04x\n", pixelformat);

	switch (pixelformat) {
	case GL_UNSIGNED_SHORT_4_4_4_4:
		s->pixfmt = GU_PSM_4444;
		break;
	case GL_UNSIGNED_SHORT_5_5_5_1:
		s->pixfmt = GU_PSM_5551;
		break;
	case GL_UNSIGNED_SHORT_5_6_5:
		s->pixfmt = GU_PSM_5650;
		break;
	case GL_RGBA:
		s->pixfmt = GU_PSM_8888;
		break;
	default:
		EGLERROR(EGL_BAD_SURFACE);
		return EGL_FALSE;
	}

	bufferlen = s->height * s->pixelperline * bytesperpixel;

	psp_log("width = %lu\n", s->width);
	psp_log("height = %lu\n", s->height);
	psp_log("pixelperline = %lu\n", s->pixelperline);
	psp_log("pixelformat = 0x%04x\n", s->pixfmt);

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

