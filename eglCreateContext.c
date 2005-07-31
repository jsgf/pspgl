#include <stdlib.h>
#include <string.h>
#include "pspgl_internal.h"


//static const struct pspgl_context default_ctx;

EGLContext eglCreateContext (EGLDisplay dpy, EGLConfig config, EGLContext share_list, const EGLint *attrib_list)
{
	struct pspgl_context *ctx = malloc(sizeof(struct pspgl_context));

	if (!ctx) {
		EGLERROR(EGL_BAD_ALLOC);
		return EGL_NO_CONTEXT;
	}

	memset(ctx, 0, sizeof(*ctx));

	ctx->dlist[0] = pspgl_dlist_create(1, pspgl_dlist_swap);
	ctx->dlist[1] = pspgl_dlist_create(1, pspgl_dlist_swap);

	ctx->dlist_current = ctx->dlist[0];

	return (EGLContext) ctx;
}
