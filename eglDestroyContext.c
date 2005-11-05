#include <stdlib.h>
#include "pspgl_internal.h"
#include "pspgl_texobj.h"

EGLBoolean eglDestroyContext (EGLDisplay dpy, EGLContext ctx)
{
	struct pspgl_context *c = (struct pspgl_context *) ctx;

	if (!c) {
		EGLERROR(EGL_BAD_CONTEXT);
		return EGL_FALSE;
	}

	if (c == __pspgl_curctx)
		__pspgl_dlist_cancel();

	__pspgl_dlist_free(c->dlist[0]);
	__pspgl_dlist_free(c->dlist[1]);

	c->refcount--;

	if (c->refcount == 0) {
		c->shared->refcount--;

		if (c->shared->refcount == 0) {
			__pspgl_hash_foreach_free(&c->shared->texture_objects, (void (*) (void *)) __pspgl_texobj_free);
			__pspgl_hash_foreach_free(&c->shared->display_lists, /* XXX should become dlist_free() */ free);
			free(c->shared);
		}

		free(c->projection_stack.stack);
		free(c->modelview_stack.stack);
		free(c->texture_stack.stack);
		free(c->view_stack.stack);

		free(c);
	}

	return EGL_TRUE;
}

