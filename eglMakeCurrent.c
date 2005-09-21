#include <stdlib.h>
#include "pspgl_internal.h"


/* mask out trigger action registers, or the GE might run amok on context changes... */
static const
uint32_t ge_reg_touch_mask [] = {
	0xffffe08f,
	0x01ffffff,
	0xffffffff,
	0xffffffff,
	0xffffffff,
	0xffffffff,
	0xfffff3ff,
	0xfefff7ff,
};


EGLBoolean eglMakeCurrent (EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx)
{
	struct pspgl_context *c = (struct pspgl_context *) ctx;
	unsigned long i;

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
		pspgl_curctx->refcount--;
	}

	pspgl_curctx = c;

	if (c->initialized) {
		/* simply mark registers and matrices as dirty, we need to rewrite them all after context restore... */
		for (i=0; i<sizeof(c->ge_reg_touched)/sizeof(c->ge_reg_touched[0]); i++)
			c->ge_reg_touched[i] = ge_reg_touch_mask[i];
		c->matrix_touched = 0x07;
	} else {
		pspgl_ge_init(c);
		c->initialized = 1;
	}

	return pspgl_vidmem_setup_write_and_display_buffer(c->draw);
}

