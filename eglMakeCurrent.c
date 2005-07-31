#include <stdlib.h>
#include <pspge.h>
#include "pspgl_internal.h"


EGLBoolean eglMakeCurrent (EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx)
{
	struct pspgl_context *c = (struct pspgl_context *) ctx;

	if (!c) {
		EGLERROR(EGL_BAD_CONTEXT);
		return EGL_FALSE;
	}

	c->refcount++;
	c->draw = draw;
	c->read = read;

	if (pspgl_curctx) {
		pspgl_dlist_finalize(pspgl_curctx->dlist_current);
		pspgl_dlist_await_completion();
		if (sceGeSaveContext((PspGeContext *) pspgl_curctx->ge_ctx) < 0) {
			EGLERROR(EGL_BAD_ACCESS);
			return EGL_FALSE;
		}
		pspgl_curctx->refcount--;
	}

	pspgl_curctx = c;

	/* start new command queue, stalled state */
	pspgl_dlist_reset(c->dlist_current);
	pspgl_dlist_submit(c->dlist_current);

	if (c->initialized) {
		if (sceGeRestoreContext((PspGeContext *) c->ge_ctx)) {
			EGLERROR(EGL_BAD_ACCESS);
			return EGL_FALSE;
		}
	} else {
		pspgl_ge_init(c);
		glScissor(0, 0, c->draw->width, c->draw->height);
		glViewport(0, 0, c->draw->width, c->draw->height);
		glDepthRangef(0.0, 1.0);
		pspgl_curctx->clear.depth = 1.0;
		c->initialized = 1;
	}

	/* XXX ?!? how to set up read buffer ?!? */
	return pspgl_vidmem_setup_write_and_display_buffer(c->draw);
}

