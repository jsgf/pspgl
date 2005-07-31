#include "pspgl_internal.h"


EGLBoolean eglSwapInterval(EGLDisplay dpy, EGLint interval)
{
	pspgl_curctx->swap_interval = interval;
	return EGL_TRUE;
}
