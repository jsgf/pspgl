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
		sendCommandf(206, distance);
		break;
	case GL_FOG_END:
		pspgl_curctx->fog.far = param;
		sendCommandf(205, pspgl_curctx->fog.far);
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
	uint32_t color;

	switch (pname) {
	case GL_FOG_COLOR:
		pspgl_curctx->fog.color[0] = param[0];
		pspgl_curctx->fog.color[1] = param[1];
		pspgl_curctx->fog.color[2] = param[2];
		pspgl_curctx->fog.color[3] = param[3];
		color  = ((int) (255.0f * CLAMPF(param[0]))) << 0;
		color |= ((int) (255.0f * CLAMPF(param[1]))) << 8;
		color |= ((int) (255.0f * CLAMPF(param[2]))) << 16;
		sendCommandi(207, color & 0xffffff);
		break;
	default:
		glFogf(pname, param[0]);
	}
}

