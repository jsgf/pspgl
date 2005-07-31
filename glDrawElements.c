#include "pspgl_internal.h"


void glDrawElements (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices)
{
	pspgl_varray_draw(mode, type, indices, 0, count);
}
