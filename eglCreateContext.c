#include <stdlib.h>
#include <string.h>
#include "pspgl_internal.h"

static void init_matrix_stack(struct pspgl_matrix_stack *mstk, int limit, unsigned flags)
{
	mstk->stack = malloc(sizeof(*mstk->stack) * limit);

	memcpy(mstk->stack[0].mat, __pspgl_identity, sizeof(mstk->stack[0].mat));

	mstk->limit = limit;
	mstk->depth = 0;
	mstk->flags = flags | MF_DIRTY;
}

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

	ctx->refcount = 1;

	/* OpenGL/ES stack size limits; we could probably afford to be more generous */
	init_matrix_stack(&ctx->projection_stack, 2, 0);
	init_matrix_stack(&ctx->texture_stack, 2, 0);
	init_matrix_stack(&ctx->modelview_stack, 16, 0);
	init_matrix_stack(&ctx->view_stack, 8, 0);
	for(i = 0; i < NBONES; i++)
		init_matrix_stack(&ctx->bone_stacks[i], 1, MF_DISABLED);
	ctx->vertexblend.enabled = GL_FALSE;

	ctx->current_matrix_stack = &ctx->modelview_stack;
	ctx->current_matrix = &ctx->modelview_stack.stack[0];

	if (share_context == NULL) {
		ctx->shared = malloc(sizeof(struct pspgl_shared_context));
		memset(ctx->shared, 0, sizeof(struct pspgl_shared_context));
	} else {
		ctx->shared = ((struct pspgl_context *) share_context)->shared;
	}

	ctx->attribstackdepth = 0;
	ctx->clattribstackdepth = 0;

	ctx->pack.alignment = 4;
	ctx->unpack.alignment = 4;

	ctx->shared->refcount++;

	return (EGLContext) ctx;
}


