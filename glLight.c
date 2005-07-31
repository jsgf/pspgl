#include "pspgl_internal.h"


void glLightfv (GLenum light, GLenum pname, const GLfloat *params)
{
	GLfloat scale;

	if (light < GL_LIGHT0 || light > GL_LIGHT3) {
		GLERROR(GL_INVALID_ENUM);
		return;
	}

	light -= GL_LIGHT0;

	switch (pname) {
	case GL_AMBIENT:
		pspgl_curctx->light[light].ambient[0] = params[0];
		pspgl_curctx->light[light].ambient[1] = params[1];
		pspgl_curctx->light[light].ambient[2] = params[2];
		sendCommandi(143+3*light, COLOR(params));
		break;
	case GL_DIFFUSE:
		pspgl_curctx->light[light].diffuse[0] = params[0];
		pspgl_curctx->light[light].diffuse[1] = params[1];
		pspgl_curctx->light[light].diffuse[2] = params[2];
		sendCommandi(144+3*light, COLOR(params));
		break;
	case GL_SPECULAR:
		pspgl_curctx->light[light].specular[0] = params[0];
		pspgl_curctx->light[light].specular[1] = params[1];
		pspgl_curctx->light[light].specular[2] = params[2];
		sendCommandi(145+3*light, COLOR(params));
		break;
	case GL_POSITION:
		pspgl_curctx->light[light].position[0] = params[0];
		pspgl_curctx->light[light].position[1] = params[1];
		pspgl_curctx->light[light].position[2] = params[2];
		pspgl_curctx->light[light].position[3] = params[3];
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
		pspgl_curctx->light[light].spot_direction[0] = params[0];
		pspgl_curctx->light[light].spot_direction[1] = params[1];
		pspgl_curctx->light[light].spot_direction[2] = params[2];
		sendCommandf(111+3*light, params[0]);
		sendCommandf(112+3*light, params[1]);
		sendCommandf(113+3*light, params[2]);
		break;
	case GL_SPOT_EXPONENT:
		/* XXX ??? seems possible, but how ?!? */
		break;
	case GL_SPOT_CUTOFF:
		/* XXX ??? seems possible, but how ?!? */
		break;
	case GL_CONSTANT_ATTENUATION:
		pspgl_curctx->light[light].constant_attentuation = params[0];
		sendCommandf(123+3*light, params[2]);
		break;
	case GL_LINEAR_ATTENUATION:
		pspgl_curctx->light[light].linear_attentuation = params[0];
		sendCommandf(124+3*light, params[2]);
		break;
	case GL_QUADRATIC_ATTENUATION:
		pspgl_curctx->light[light].quadratic_attentuation = params[0];
		sendCommandf(125+3*light, params[2]);
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

