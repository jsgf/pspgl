#include <stdlib.h>
#include <string.h>
#include "pspgl_internal.h"

EGLContext eglCreateContext (EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint *attrib_list)
{
	struct pspgl_context *ctx = malloc(sizeof(struct pspgl_context));
	int i;

	if (!ctx) {
		EGLERROR(EGL_BAD_ALLOC);
		return EGL_NO_CONTEXT;
	}

	psp_log("created context %p\n", ctx);

	memset(ctx, 0, sizeof(*ctx));

	for(i = 0; i < NUM_CMDLISTS; i++)
		ctx->dlist[i] = __pspgl_dlist_create(1, __pspgl_dlist_swap);

	ctx->dlist_current = ctx->dlist[0];
	ctx->dlist_idx = 0;

	if (share_context == NULL) {
		ctx->shared = malloc(sizeof(struct pspgl_shared_context));
		memset(ctx->shared, 0, sizeof(struct pspgl_shared_context));
	} else {
		ctx->shared = ((struct pspgl_context *) share_context)->shared;
	}

	ctx->shared->refcount++;

	return (EGLContext) ctx;
}


