#include "pspgl_internal.h"


void glLightfv (GLenum light, GLenum pname, const GLfloat *params)
{
	struct pspgl_context *c = pspgl_curctx;
	GLfloat scale;

	if (light < GL_LIGHT0 || light > GL_LIGHT3) {
		GLERROR(GL_INVALID_ENUM);
		return;
	}

	light -= GL_LIGHT0;

	switch (pname) {
	case GL_AMBIENT:
		sendCommandi(143+3*light, COLOR3(params));
		break;
	case GL_DIFFUSE:
		sendCommandi(144+3*light, COLOR3(params));
		break;
	case GL_SPECULAR:
		sendCommandi(145+3*light, COLOR3(params));
		break;
	case GL_POSITION:
		scale = 1.0 / params[3];
		if (scale > 0 && scale < 0.001)
			scale = 0.001;
		else if (scale < 0 && scale > -0.001)
			scale = -0.001;
		sendCommandf(99+3*light, scale * params[0]);
		sendCommandf(100+3*light, scale * params[1]);
		sendCommandf(101+3*light, scale * params[2]);
		break;
	case GL_SPOT_DIRECTION:
		sendCommandf(111+3*light, params[0]);
		sendCommandf(112+3*light, params[1]);
		sendCommandf(113+3*light, params[2]);
		break;
	case GL_SPOT_EXPONENT:
		sendCommandf(139+light, params[0]);
		break;
	case GL_SPOT_CUTOFF:
		pspgl_context_writereg_masked(c, 95+light, ((params[0] == 0.0) ? 0 : (params[0] == 180.0) ? 1 : 2) << 8, 0x00ff00);
		sendCommandf(135+light, params[0]);
		break;
	case GL_CONSTANT_ATTENUATION:
		sendCommandf(123+3*light, params[0]);
		break;
	case GL_LINEAR_ATTENUATION:
		sendCommandf(124+3*light, params[0]);
		break;
	case GL_QUADRATIC_ATTENUATION:
		sendCommandf(125+3*light, params[0]);
		break;
	default:
		GLERROR(GL_INVALID_ENUM);
		return;
	}
}


void glLightf (GLenum light, GLenum pname, GLfloat param)
{
	glLightfv(light, pname, &param);
}

