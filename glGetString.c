#include <stdlib.h>
#include "pspgl_internal.h"


static const char *vendor_string = "pspGL";
static const char *version_string = "OpenGL ES-CM 1.1";
static const char *renderer_string = "(pspGL build " __DATE__ ", " __TIME__ ")";
static const char *extension_string = "EXT_texture_env_add ARB_texture_env_add "
				      "EXT_blend_minmax EXT_blend_subtract ";

const GLubyte * glGetString( GLenum name )
{
	const char *s;

	switch (name) {
	case GL_VENDOR:
		s = vendor_string;
		break;
	case GL_RENDERER:
		s = renderer_string;
		break;
	case GL_VERSION:
		s = version_string;
		break;
	case GL_EXTENSIONS:
		s = extension_string;
		break;
	default:
		EGLERROR(EGL_BAD_PARAMETER);
		s = NULL;
	}

	return (const GLubyte *) s;
}
