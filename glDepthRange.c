#include "pspgl_internal.h"


void glDepthRangef (GLclampf zNear, GLclampf zFar)
{
	/* z Scale and Offset */
	sendCommandf(CMD_VIEWPORT_SZ, (zNear - zFar) / 2);
	sendCommandf(CMD_VIEWPORT_TZ, (zNear + zFar) / 2 + pspgl_curctx->depth_offset);

	if (zNear > zFar) {
		GLfloat temp = zNear;
		zNear = zFar;
		zFar = temp;
	}

	/* Far and Near Clip Planes */
	sendCommandi(CMD_CLIP_NEAR, (int) zNear);
	sendCommandi(CMD_CLIP_FAR, (int) zFar);
}



void glDepthRange (GLclampd zNear, GLclampd zFar)
{
	glDepthRangef(zNear, zFar);
}

