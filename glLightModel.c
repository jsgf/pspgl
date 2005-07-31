#include "pspgl_internal.h"


void glLightModelfv (GLenum pname, const GLfloat *params)
{
	switch (pname) {
	case GL_LIGHT_MODEL_AMBIENT:
		pspgl_curctx->light_model.ambient[0] = params[0];
		pspgl_curctx->light_model.ambient[1] = params[1];
		pspgl_curctx->light_model.ambient[2] = params[2];
		pspgl_curctx->light_model.ambient[3] = params[3];
		sendCommandi(92, COLOR(params));
		sendCommandi(93, (int) (255.0 * CLAMPF(params[3])));
		break;
	case GL_LIGHT_MODEL_COLOR_CONTROL:
		pspgl_curctx->light_model.color_control = params[0];
		sendCommandi(94, (params[0] == GL_SEPARATE_SPECULAR_COLOR) ? 1 : 0);
		break;
	default:
		GLERROR(GL_INVALID_ENUM);
		return;
	}
}


void glLightModelf (GLenum pname, GLfloat param)
{
	glLightModelfv(pname, &param);
}

