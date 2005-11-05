#include "pspgl_internal.h"


void glMaterialfv (GLenum face, GLenum pname, const GLfloat *params)
{
	switch (pname) {
	case GL_AMBIENT:
		sendCommandi(CMD_MATERIAL_AMB_C, COLOR3(params));
		sendCommandi(CMD_MATERIAL_AMB_A, (int) (255.0 * CLAMPF(params[3])));
		break;
	case GL_EMISSION:
		sendCommandi(CMD_MATERIAL_EMS_C, COLOR3(params));
		break;
	case GL_DIFFUSE:
		sendCommandi(CMD_MATERIAL_DIF_C, COLOR3(params));
		break;
	case GL_AMBIENT_AND_DIFFUSE:
		glMaterialfv(face, GL_AMBIENT, params);
		glMaterialfv(face, GL_DIFFUSE, params);
		break;
	case GL_SPECULAR:
		sendCommandi(CMD_MATERIAL_SPC_C, COLOR3(params));
		break;
	case GL_SHININESS:
		sendCommandf(CMD_MATERIAL_SPEC_POW, params[0]);
		break;
	default:
		GLERROR(GL_INVALID_ENUM);
		return;
	}
}


void glMaterialf (GLenum face, GLenum pname, GLfloat param)
{
	glGetMaterialfv(face, pname, &param);
}

