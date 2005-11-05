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
		sendCommandi(CMD_LIGHT0_AMB_COL+3*light, COLOR3(params));
		break;
	case GL_DIFFUSE:
		sendCommandi(CMD_LIGHT0_DIF_COL+3*light, COLOR3(params));
		break;
	case GL_SPECULAR:
		sendCommandi(CMD_LIGHT0_SPC_COL+3*light, COLOR3(params));
		break;
	case GL_POSITION:
		scale = 1.0 / params[3];
		if (scale > 0 && scale < 0.001)
			scale = 0.001;
		else if (scale < 0 && scale > -0.001)
			scale = -0.001;
		sendCommandf(CMD_LIGHT0_POS_X+3*light, scale * params[0]);
		sendCommandf(CMD_LIGHT0_POS_Y+3*light, scale * params[1]);
		sendCommandf(CMD_LIGHT0_POS_Z+3*light, scale * params[2]);
		break;
	case GL_SPOT_DIRECTION:
		sendCommandf(CMD_LIGHT0_VEC_X+3*light, params[0]);
		sendCommandf(CMD_LIGHT0_VEC_Y+3*light, params[1]);
		sendCommandf(CMD_LIGHT0_VEC_Z+3*light, params[2]);
		break;
	case GL_SPOT_EXPONENT:
		sendCommandf(CMD_LIGHT0_SPOT_EXP+light, params[0]);
		break;
	case GL_SPOT_CUTOFF:
		pspgl_context_writereg_masked(c, CMD_LIGHT0_TYPE+light,
					      ((params[0] == 0.0) ? 0 : (params[0] == 180.0) ? 1 : 2) << 8, 0x00ff00);
		sendCommandf(CMD_LIGHT0_CUTOFF+light, params[0]);
		break;
	case GL_CONSTANT_ATTENUATION:
		sendCommandf(CMD_LIGHT0_ATT_CONST+3*light, params[0]);
		break;
	case GL_LINEAR_ATTENUATION:
		sendCommandf(CMD_LIGHT0_ATT_LINEAR+3*light, params[0]);
		break;
	case GL_QUADRATIC_ATTENUATION:
		sendCommandf(CMD_LIGHT0_ATT_QUAD+3*light, params[0]);
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

