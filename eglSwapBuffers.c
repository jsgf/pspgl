#include "pspgl_internal.h"


EGLBoolean eglSwapBuffers (EGLDisplay dpy, EGLSurface draw)
{
	struct pspgl_surface *s = (struct pspgl_surface *) draw;

	if (s->color_buffer[1])
		s->current_front ^= 1;
	else
		s->current_front = 0;

	return pspgl_vidmem_setup_write_and_display_buffer(s);
}
