#include <stdlib.h>
#include "pspgl_internal.h"


/**
 * mask out trigger action registers, or the GE might run amok on context changes...
 * This bitfield is generated from ge_init_state[] with all non-action fields enabled.
 */
static const
uint32_t ge_reg_touch_mask [] = {
	0x6000b7ff,
	0xffbff380,
	0x3ffcdf9f,
	0xffffffff,
	0xffffffff,
	0xfffffcff,
	0xffffbfff,
	0xffdaffc0
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
		__pspgl_dlist_finalize(pspgl_curctx->dlist_current);
		__pspgl_dlist_await_completion();
		pspgl_curctx->refcount--;
	}

	pspgl_curctx = c;

	if (!c->initialized) {
		__pspgl_ge_init(c);
		c->initialized = 1;
	}

	/* mark all registers and matrices as dirty, we need to rewrite them at init and after context restore... */
	for (i=0; i<sizeof(c->ge_reg_touched)/sizeof(c->ge_reg_touched[0]); i++)
		c->ge_reg_touched[i] |= ge_reg_touch_mask[i];
	
	c->projection_stack.flags |= MF_DIRTY;
	c->modelview_stack.flags |= MF_DIRTY;
	c->texture_stack.flags |= MF_DIRTY;
	for(i = 0; i < NBONES; i++)
		c->bone_stacks[i].flags |= MF_DIRTY;

	return __pspgl_vidmem_setup_write_and_display_buffer(c->draw);
}

