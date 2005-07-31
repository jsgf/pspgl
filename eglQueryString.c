#include <stdlib.h>
#include "pspgl_internal.h"


static const char *vendor_string = "pspEGL";
static const char *version_string = "1.1 (pspEGL build " __DATE__ ", " __TIME__ ")";
static const char *extension_string = "";

const char* eglQueryString (EGLDisplay dpy, EGLint name)
{
	const char *s;

	switch (name) {
	case EGL_VENDOR:
		s = vendor_string;
		break;
	case EGL_VERSION:
		s = version_string;
		break;
	case EGL_EXTENSIONS:
		s = extension_string;
		break;
	default:
		EGLERROR(EGL_BAD_PARAMETER);
		s = NULL;
	}

	return s;
}
