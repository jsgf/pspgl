#include "pspgl_internal.h"


void glMaterialfv (GLenum face, GLenum pname, const GLfloat *params)
{
	switch (pname) {
	case GL_AMBIENT:
		pspgl_curctx->material.ambient[0] = params[0];
		pspgl_curctx->material.ambient[1] = params[1];
		pspgl_curctx->material.ambient[2] = params[2];
		pspgl_curctx->material.ambient[3] = params[3];
		sendCommandi(85, COLOR(params));
		sendCommandi(88, (int) (255.0 * CLAMPF(params[3])));
		break;
	case GL_DIFFUSE:
		pspgl_curctx->material.diffuse[0] = params[0];
		pspgl_curctx->material.diffuse[1] = params[1];
		pspgl_curctx->material.diffuse[2] = params[2];
		pspgl_curctx->material.diffuse[3] = params[3];
		sendCommandi(84, COLOR(params));
		sendCommandi(87, (int) (255.0 * CLAMPF(params[3])));
		break;
	case GL_AMBIENT_AND_DIFFUSE:
		glMaterialfv(face, GL_AMBIENT, params);
		glMaterialfv(face, GL_DIFFUSE, params);
		break;
	case GL_SPECULAR:
		pspgl_curctx->material.specular[0] = params[0];
		pspgl_curctx->material.specular[1] = params[1];
		pspgl_curctx->material.specular[2] = params[2];
		pspgl_curctx->material.specular[3] = params[3];
		sendCommandi(83, COLOR(params));
		sendCommandi(86, (int) (255.0 * CLAMPF(params[3])));
		break;
	case GL_SHININESS:
		pspgl_curctx->material.shininess = params[0];
		sendCommandf(91, params[0]);
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

