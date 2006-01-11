#include "pspgl_internal.h"


EGLBoolean eglSwapBuffers (EGLDisplay dpy, EGLSurface draw)
{
	struct pspgl_surface *s = (struct pspgl_surface *) draw;

	s->current_front ^= 1;

	return __pspgl_vidmem_setup_write_and_display_buffer(s);
}
