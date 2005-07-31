#include "pspgl_internal.h"


void glDepthRangef (GLclampf zNear, GLclampf zFar)
{
	/* z Scale and Offset */
	sendCommandf(68, (zFar - zNear) / 2);
	sendCommandf(71, (zNear + zFar) / 2 + pspgl_curctx->depth_offset);

	if (zNear > zFar) {
		GLfloat temp = zNear;
		zNear = zFar;
		zFar = temp;
	}

	/* Far and Near Clip Planes */
	sendCommandi(214, (int) zNear);
	sendCommandi(215, (int) zFar);
}

