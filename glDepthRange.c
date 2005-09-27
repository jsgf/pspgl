#include "pspgl_internal.h"


void glDepthRangef (GLclampf zNear, GLclampf zFar)
{
	zNear = 65535.0f * CLAMPF(zNear);
	zFar = 65535.0f * CLAMPF(zFar);

	/* z Scale */
	sendCommandf(68, (zFar - zNear) / 2);
//	sendCommandf(68, 0);

	/* z Offset */
//with	sendCommandf(68, 0);
//ffff	sendCommandf(71, 65535.0 /*(zNear + zFar) / 2 + pspgl_curctx->depth_offset*/);
//0000	sendCommandf(71, 0.0 /*(zNear + zFar) / 2 + pspgl_curctx->depth_offset*/);

	sendCommandf(71, 65535.0 /*(zNear + zFar) / 2 + pspgl_curctx->depth_offset*/);

	/* Far and Near Clip Planes */
	sendCommandi(214, 0);
	sendCommandi(215, 65535);
//	sendCommandf(214, -1.0);
//	sendCommandf(215, 1.0);
}


void glDepthRange (GLclampd zNear, GLclampd zFar)
{
	glDepthRangef(zNear, zFar);
}

