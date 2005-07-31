#include "pspgl_internal.h"


void glFlush (void)
{
	pspgl_dlist_swap(pspgl_curctx->dlist_current);
}
