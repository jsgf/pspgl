#include "pspgl_internal.h"


void glFinish (void)
{
	__pspgl_dlist_swap(pspgl_curctx->dlist_current);
	__pspgl_dlist_await_completion();
}
