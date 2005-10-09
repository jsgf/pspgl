#include "pspgl_internal.h"


void gluPerspectivef (GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar);


void gluPerspective (GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
	gluPerspectivef(fovy, aspect, zNear, zFar);
}

