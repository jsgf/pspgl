#include "pspgl_internal.h"


void glDrawArrays (GLenum mode, GLint first, GLsizei count)
{
	__pspgl_varray_draw(mode, 0, (void*) 0, first, count);
}
