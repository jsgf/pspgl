#include "pspgl_internal.h"


void glFogf (GLenum pname, GLfloat param)
{
	float distance;

	switch (pname) {
	case GL_FOG_MODE:
		if (param != GL_LINEAR)
			GLERROR(GL_INVALID_VALUE);
		break;
	case GL_FOG_START:
		pspgl_curctx->fog.near = param;
		distance = pspgl_curctx->fog.far - pspgl_curctx->fog.near;
		if (distance != 0)
			distance = 1.0f / distance;
		else
			GLERROR(GL_INVALID_VALUE);
		sendCommandf(CMD_FOG_NEAR, distance);
		break;
	case GL_FOG_END:
		pspgl_curctx->fog.far = param;
		sendCommandf(CMD_FOG_FAR, pspgl_curctx->fog.far);
		break;
	/**
	case XXXX:
		pspgl_curctx->fog.XXXXX = param;
		sendCommandf(248, ??fog type??);
		break;
	 */
	default:
		GLERROR(GL_INVALID_ENUM);
	}
}


void glFogi (GLenum pname, GLint param)
{
	glFogf(pname, (GLfloat) param);
}


void glFogfv (GLenum pname, const GLfloat *param)
{
	switch (pname) {
	case GL_FOG_COLOR:
		sendCommandi(CMD_FOG_COLOR, COLOR3(param));
		break;
	default:
		glFogf(pname, param[0]);
	}
}

