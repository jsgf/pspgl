#include <math.h>
#include "pspgl_internal.h"


static inline
void cross_product (GLfloat a [3], GLfloat b [3], GLfloat product [3])
{
	product[0] = a[1] * b[2] - a[2] * b[1];
	product[1] = a[2] * b[0] - a[0] * b[2];
	product[2] = a[0] * b[1] - a[1] * b[0];
}


static inline
void normalize (GLfloat v [3])
{
	GLfloat len = sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

	if (len != 0.0) {
		GLfloat scale = 1.0f / len;
		v[0] *= scale;
		v[1] *= scale;
		v[2] *= scale;
	}
}


/**
 *  Variable names like in 'man gluLookAt'
 */
void gluLookAt (GLfloat eyeX, GLfloat eyeY, GLfloat eyeZ,
		GLfloat centerX, GLfloat centerY, GLfloat centerZ,
		GLfloat upX, GLfloat upY, GLfloat upZ)
{
	GLfloat m [16];
	GLfloat f [3];
	GLfloat u [3];
	GLfloat s [3];

	f[0] = centerX - eyeX;
	f[1] = centerY - eyeY;
	f[2] = centerZ - eyeZ;

	u[0] = upX;
	u[1] = upY;
	u[2] = upZ;

	normalize(f);
	cross_product(f, u, s);
	normalize(s);
	cross_product(s, f, u);

	m[0] =  s[0]; m[4] =  s[1]; m[8] =   s[2]; m[12] = 0.0;
	m[1] =  u[0]; m[5] =  u[1]; m[9] =   u[2]; m[13] = 0.0;
	m[2] = -f[0]; m[6] = -f[1]; m[10] = -f[2]; m[14] = 0.0;
	m[3] =   0.0; m[7] =   0.0; m[11] =   0.0; m[15] = 1.0;

	glMultMatrixf(m);
	glTranslatef(-eyeX, -eyeY, -eyeZ);
}

