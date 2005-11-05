#include <pspkernel.h>
#include "pspgl_internal.h"


EGLBoolean eglTerminate (EGLDisplay dpy)
{
	psp_log("\n\n=============== pspGL, build " __DATE__ ", " __TIME__ " ===============\n");

	if (__pspgl_curctx)
		__pspgl_dlist_cancel();

	return EGL_TRUE;
}
