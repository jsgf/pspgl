#include <stdlib.h>
#include "pspgl_internal.h"


static const
struct pspgl_pixelformat {
	unsigned int pixelformat : 16;
	unsigned int red_bits : 4;
	unsigned int green_bits: 4;
	unsigned int blue_bits : 4;
	unsigned int alpha_bits : 4;
} pspgl_pixelformat_list [] = {
	/* sorted by increasing alpha_bits */
	{ .pixelformat = GL_UNSIGNED_SHORT_5_6_5,   5, 6, 5, 0 },
	{ .pixelformat = GL_UNSIGNED_SHORT_5_5_5_1, 5, 5, 5, 1 },
	{ .pixelformat = GL_UNSIGNED_SHORT_4_4_4_4, 4, 4, 4, 4 },
	{ .pixelformat = GL_RGBA,                   8, 8, 8, 8 }
};



EGLBoolean eglChooseConfig (EGLDisplay dpy, const EGLint *attrib_list, EGLConfig *configs, EGLint config_size, EGLint *num_config)
{
	int alpha_size = 0, blue_size = 0, green_size = 0, red_size = 0, depth_size = 0, stencil_size = 0;
	int pixelformat = -1;
	int i;

	while (*attrib_list != EGL_NONE) {
		switch (*attrib_list) {
		case EGL_BUFFER_SIZE: break;
		case EGL_ALPHA_SIZE: alpha_size = attrib_list[1]; break;
		case EGL_BLUE_SIZE: blue_size = attrib_list[1]; break;
		case EGL_GREEN_SIZE: green_size = attrib_list[1]; break;
		case EGL_RED_SIZE: red_size = attrib_list[1]; break;
		case EGL_DEPTH_SIZE: depth_size = attrib_list[1]; break;
		case EGL_STENCIL_SIZE: stencil_size = attrib_list[1]; break;
		case EGL_CONFIG_CAVEAT:
		case EGL_CONFIG_ID:
		case EGL_LEVEL:
		case EGL_MAX_PBUFFER_HEIGHT:
		case EGL_MAX_PBUFFER_PIXELS:
		case EGL_MAX_PBUFFER_WIDTH:
		case EGL_NATIVE_RENDERABLE:
		case EGL_NATIVE_VISUAL_ID:
		case EGL_NATIVE_VISUAL_TYPE:
		case EGL_SAMPLES:
		case EGL_SAMPLE_BUFFERS:
		case EGL_SURFACE_TYPE:
		case EGL_TRANSPARENT_TYPE:
		case EGL_TRANSPARENT_BLUE_VALUE:
		case EGL_TRANSPARENT_GREEN_VALUE:
		case EGL_TRANSPARENT_RED_VALUE:
		case EGL_BIND_TO_TEXTURE_RGB:
		case EGL_BIND_TO_TEXTURE_RGBA:
		case EGL_MIN_SWAP_INTERVAL:
		case EGL_MAX_SWAP_INTERVAL:
			break;
		default:
			EGLERROR(EGL_BAD_ATTRIBUTE);
		}

		attrib_list += 2;
	}

	for (i=0; i<sizeof(pspgl_pixelformat_list)/sizeof(pspgl_pixelformat_list[0]); i++) {
		if (red_size > pspgl_pixelformat_list[i].red_bits)
			continue;
		if (green_size > pspgl_pixelformat_list[i].green_bits)
			continue;
		if (blue_size > pspgl_pixelformat_list[i].blue_bits)
			continue;
		if (alpha_size > pspgl_pixelformat_list[i].alpha_bits)
			continue;
		pixelformat = pspgl_pixelformat_list[i].pixelformat;
		break;
	}

	if (pixelformat == -1 || depth_size > 16 || stencil_size > 16) {
		EGLERROR(EGL_BAD_ATTRIBUTE);
		return EGL_FALSE;
	}

	if (config_size >= 1) {
		configs[0] = (int) pixelformat;
		*num_config = 1;
	}

	return EGL_TRUE;
}

