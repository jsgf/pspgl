#include <stdlib.h>
#include "pspgl_internal.h"


EGLBoolean eglDestroyContext (EGLDisplay dpy, EGLContext ctx)
{
	struct pspgl_context *c = (struct pspgl_context *) ctx;

	if (!c) {
		EGLERROR(EGL_BAD_CONTEXT);
		return EGL_FALSE;
	}

	if (c == pspgl_curctx)
		pspgl_dlist_cancel();

	pspgl_dlist_free(c->dlist[0]);
	pspgl_dlist_free(c->dlist[1]);

	c->refcount--;

	if (c->refcount == 0) {
		int i;

		for (i=0; i<3; i++) {
			if (c->matrix_stack[i])
				free(c->matrix_stack[i]);
		}

		c->shared->refcount--;

		if (c->shared->refcount == 0) {
			if (c->shared->texobj_list)
				free(c->shared->texobj_list);
			free(c->shared);
		}

		free(c);
	}

	return EGL_TRUE;
}

