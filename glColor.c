#include "pspgl_internal.h"


void glColor4fv (const GLfloat *color)
{
	pspgl_curctx->current.color = COLOR4(color);
}


void glColor4f (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
	pspgl_curctx->current.color = ((((int) (255.0 * CLAMPF(alpha))) << 24) |
				       (((int) (255.0 * CLAMPF(blue))) << 16) |
				       (((int) (255.0 * CLAMPF(green))) << 8) |
					((int) (255.0 * CLAMPF(red))));
}


void glColor3f (GLfloat red, GLfloat green, GLfloat blue)
{
	glColor4f(red, green, blue, 1.0);
}


void glColor4ubv (const GLubyte *c)
{
	pspgl_curctx->current.color = (c[3] << 24) | (c[2] << 16) | (c[1] << 8) | c[0];
}


void glColor3ub (GLubyte r, GLubyte g, GLubyte b)
{
	pspgl_curctx->current.color = 0xff000000 | (b << 16) | (g << 8) | b;
}

 
void glColor4ub (GLubyte r, GLubyte g, GLubyte b, GLubyte a)
{
	pspgl_curctx->current.color = (a << 24) | (b << 16) | (g << 8) | b;
}

