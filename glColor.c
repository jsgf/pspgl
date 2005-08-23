#include "pspgl_internal.h"


void glColor4fv (const GLfloat *color)
{
	pspgl_curctx->current.color = COLOR4(color);
}


void glColor4f (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
	unsigned long c = ((((int) (255.0 * CLAMPF(alpha))) << 24) |
			   (((int) (255.0 * CLAMPF(blue))) << 16) |
			   (((int) (255.0 * CLAMPF(green))) << 8) |
			    ((int) (255.0 * CLAMPF(red))));

	pspgl_curctx->current.color = c;
}


void glColor3f (GLfloat red, GLfloat green, GLfloat blue)
{
	glColor4f(red, green, blue, 1.0);
}


void glColor4ubv (const GLubyte *c)
{
	pspgl_curctx->current.color = (c[3] << 24) | (c[2] << 16) | (c[1] << 8) | c[0];
}

