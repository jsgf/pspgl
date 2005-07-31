#include "pspgl_internal.h"


void glFinish (void)
{
	pspgl_dlist_finalize(pspgl_curctx->dlist_current);
	pspgl_dlist_await_completion();
	pspgl_dlist_reset(pspgl_curctx->dlist_current);
	pspgl_dlist_submit(pspgl_curctx->dlist_current);
}
