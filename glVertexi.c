#include "pspgl_internal.h"


static inline GLfloat i2f (GLint i) { return (GLfloat) i * (1.0 / 32768.0); }


void glVertex3i (GLint x, GLint y, GLint z)
{
	glVertex3f(i2f(x), i2f(y), i2f(z));
}


void glVertex2i (GLint x, GLint y)
{
	glVertex3f(i2f(x), i2f(y), 0.0);
}

