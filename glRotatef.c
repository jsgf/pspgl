#include <math.h>
#include "pspgl_internal.h"


void glRotatef (GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
	GLfloat xx, yy, zz, xy, yz, zx, xs, ys, zs, s, c, one_minus_c, scale;
	GLfloat m [16];

	angle *= (M_PI/180.0);

	s = sinf(angle);
	c = cosf(angle);

	xx = x * x;
	yy = y * y;
	zz = z * z;

	scale = sqrtf(xx + yy + zz);

	if (scale < 1.0e-4)
		return;

	scale = 1.0 / scale;

	x *= scale;
	y *= scale;
	z *= scale;

	xy = x * y;
	yz = y * z;
	zx = z * x;

	xs = x * s;
	ys = y * s;
	zs = z * s;

	one_minus_c = 1.0 - c;

	m[0] = (one_minus_c * xx) + c;
	m[1] = (one_minus_c * xy) + zs;
	m[2] = (one_minus_c * zx) - ys;
	m[3] = 0.0;

	m[4] = (one_minus_c * xy) - zs;
	m[5] = (one_minus_c * yy) + c;
	m[6] = (one_minus_c * yz) + xs;
	m[7] = 0.0;

	m[8] = (one_minus_c * zx) + ys;
	m[9] = (one_minus_c * yz) - xs;
	m[10] = (one_minus_c * zz) + c;
	m[11] = 0.0;

	m[12] = 0.0;
	m[13] = 0.0;
	m[14] = 0.0;
	m[15] = 1.0;

	glMultMatrixf(m);
}
