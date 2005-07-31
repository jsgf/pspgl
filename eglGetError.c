#include "pspgl_internal.h"


EGLint eglGetError (void)
{
	EGLint err = eglerror;
	eglerror = EGL_SUCCESS;
	return err;
}
