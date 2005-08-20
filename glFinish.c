#include "pspgl_internal.h"


void glFinish (void)
{
	pspgl_dlist_swap(pspgl_curctx->dlist_current);
	pspgl_dlist_await_completion();
}
