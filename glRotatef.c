#include <math.h>

#include <pspvfpu.h>

#include "pspgl_internal.h"

void glRotatef (GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
#if 0
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
#else
	struct pspgl_context *c = pspgl_curctx;
	struct pspgl_matrix_stack *stk = c->current_matrix_stack;
	struct pspgl_matrix *mat = c->current_matrix;

	if (!(stk->flags & MF_DISABLED))
		stk->flags |= MF_DIRTY;

	mat->flags &= ~MF_IDENTITY;

	pspvfpu_use_matrices(c->vfpu_context, VFPU_STACKTOP, VMAT6 | VMAT5);

	/* 
	   Initial rotate matrix is set up:
	   | 1 0 0 x |
	   | 0 1 0 y |
	   | 0 0 1 z |
	   | t 0 0 a |

	   It's really only 3x3, but we're using the 4th row and col
	   for storing useful stuff.
	 */
	asm volatile("vmidt.q	m600\n"
		     "mtv	%0,s630\n"
		     "mtv	%1,s631\n"
		     "mtv	%2,s632\n"
		     "mtv	%3,s633\n"
		     : : "r" (x), "r" (y), "r" (z), "r" (angle * (1.f / 90.f)));

	/* Look for simple rotates around an axis.  Also handles
	   0-length vectors (by doing something ill-defined). */
	if (x == 0.f && y == 0.f) {
		if (z < 0.f)
			asm volatile("vrot.p r600,s633,[c,s]\n"
				     "vrot.p r601,s633,[-s,c]\n");
		else
			asm volatile("vrot.p r600,s633,[c,-s]\n"
				     "vrot.p r601,s633,[s,c]\n");
	} else if (x == 0.f && z == 0.f) {
		if (y < 0.f)
			asm volatile("vrot.t r600,s633,[c,0,-s]\n"
				     "vrot.t r602,s633,[s,0,c]\n");
		else
			asm volatile("vrot.t r600,s633,[c,0,s]\n"
				     "vrot.t r602,s633,[-s,0,c]\n");
	} else if (y == 0.f && z == 0.f) {
		if (x < 0.f)
			asm volatile("vrot.t r601,s633,[0,c,s]\n"
				     "vrot.t r602,s633,[0,-s,c]\n");
		else
			asm volatile("vrot.t r601,s633,[0,c,-s]\n"
				     "vrot.t r602,s633,[0,s,c]\n");
	} else {
		/*
		  Complex case.  The full matrix we're generating is:

		  | xx(1-c)+ c xy(1-c)-zs xz(1-c)+ys |
		  | yx(1-c)+zs yy(1-c)+ c yz(1-c)-xs |
		  | zx(1-c)-ys zy(1-c)+xs zz(1-c)+ c |
		 */
		
		/* normalize (x,y,z) */
		asm volatile("vdot.t s603,c630,c630\n"			// t = x*x+y*y+z*z
			     "vrsq.s s603,s603\n"			// t = 1/sqrt(t)
			     "vscl.t c630[-1:1,-1:1,-1:1],c630,s603\n"	// (x,y,z) = (x*t, y*t, z*t)
			);

		/* 
		   Populate matrix 6 with sin/cos terms:
		   |  c -s  s |
		   |  s  c -s |
		   | -s  s  c |

		   vrot isn't terribly useful here because it can't
		   generate -s and s at the same time, but we can
		   still use it as sincos
		 */
		asm volatile("vrot.p r600, s633, [c, -s]\n"
			     "vneg.s s620, s610\n"		// r600 (x,y,z) = (c, -s, s)
			     "vmov.t r601, r600[z,x,y]\n"
			     "vmov.t r602, r600[y,z,x]\n"

			     /* 
				Multiply in x,y,z:
				| 1 z y |    |  c -zs  ys |
				| z 1 x | -> | zs   c -xs |
				| y x 1 |    |-ys  xs   c |
			      */
			     "vmul.t r600, r600, c630[1,z,y]\n"
			     "vmul.t r601, r601, c630[z,1,x]\n"
			     "vmul.t r602, r602, c630[y,x,1]\n"
			);

		asm volatile("vocp.s	s603,s600\n"		// s603 = 1-c
			     "vscl.t	r503,c630,s603\n"	// r503 = (1-c)(x, y, z)

			     /* Set up the xyz pairings in matrix 5:
				| x x x | | x y z |          | xx(1-c) xy(1-c) xz(1-c) |
				| y y y | | x y z | (1-c) -> | yx(1-c) yy(1-c) yz(1-c) |
				| z z z | | x y z |          | zx(1-c) zy(1-c) zz(1-c) |
			     */
			     "vscl.t	r500,r503,s630\n"	// x [(1-c)x, (1-c)y, (1-c)z]
			     "vscl.t	r501,r503,s631\n"	// y [(1-c)x, (1-c)y, (1-c)z]
			     "vscl.t	r502,r503,s632\n"	// z [(1-c)x, (1-c)y, (1-c)z]

			     /* Add 5 into 6
				| xx(1-c) xy(1-c) xz(1-c) |   |  c zs  ys |
				| yz(1-c) yy(1-c) yz(1-c) | + | zs  c -xs |
				| zx(1-c) zy(1-c) zz(1-c) |   |-ys xs   c |

				  -> 

				| xx(1-c)+ c xy(1-c)-zs xz(1-c)+ys |
				| yx(1-c)+zs yy(1-c)+ c yz(1-c)-xs |
				| zx(1-c)-ys zy(1-c)+xs zz(1-c)+ c |

			      */
			     "vadd.t	r600,r600,r500\n"
			     "vadd.t	r601,r601,r501\n"
			     "vadd.t	r602,r602,r502\n"
			);
	}

	/* Finally, multiply the rotation onto the top of the stack */
	asm volatile("vmmul.t	m500,m700,m600\n"
		     "vmmov.t	m700,m500\n");
#endif
}
