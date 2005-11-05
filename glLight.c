#include "pspgl_internal.h"

static void set_light_type(int light)
{
	struct pspgl_context *c = pspgl_curctx;
	unsigned type;
	unsigned components;
	unsigned kind;

	if (c->lights.positional[light]) {
		if (c->lights.spotlight[light])
			type = GE_SPOTLIGHT;
		else
			type = GE_POINTLIGHT;
	} else
		type = GE_DIRECTIONAL;

	components = 0;
	if (getReg(CMD_LIGHT0_AMB_COL+light) & 0xffffff)
		components |= GE_AMBIENT;
	if (getReg(CMD_LIGHT0_DIF_COL+light) & 0xffffff)
		components |= GE_DIFFUSE;
	if (getReg(CMD_LIGHT0_SPC_COL+light) & 0xffffff)
		components |= GE_SPECULAR;

	/* I got this from sceGuLight(); who knows what it means... */
	kind = (components ^ 6) < 1;

	psp_log("light %d, type=%d components=%d kind=%d\n", 
		light, type, components, kind);

	sendCommandi(CMD_LIGHT0_TYPE+light, (type << 8) | kind);
}

/*
 * Transform a point (column vector) by a matrix:   Q = M * P
 */
#define TRANSFORM_POINT( Q, M, P )					\
   Q[0] = M[0] * P[0] + M[4] * P[1] + M[8] *  P[2] + M[12] * P[3];	\
   Q[1] = M[1] * P[0] + M[5] * P[1] + M[9] *  P[2] + M[13] * P[3];	\
   Q[2] = M[2] * P[0] + M[6] * P[1] + M[10] * P[2] + M[14] * P[3];	\
   Q[3] = M[3] * P[0] + M[7] * P[1] + M[11] * P[2] + M[15] * P[3];

void glLightfv (GLenum light, GLenum pname, const GLfloat *params)
{
	struct pspgl_context *c = pspgl_curctx;

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
	case GL_POSITION: {
		const struct pspgl_matrix_stack *mv = &c->modelview_stack;
		float eye[4];

		/* Transform light into eye-space using the current
		   modelview matrix.  This is done once at
		   specification time. */
		TRANSFORM_POINT(eye, mv->stack[mv->depth].mat, params);

		pspgl_curctx->lights.positional[light] = (eye[3] != 0.);

		sendCommandf(CMD_LIGHT0_POS_X+3*light, eye[0]);
		sendCommandf(CMD_LIGHT0_POS_Y+3*light, eye[1]);
		sendCommandf(CMD_LIGHT0_POS_Z+3*light, eye[2]);
		break;
	}
	case GL_SPOT_DIRECTION:
		/* XXX need to transform by inverse modelview */
		sendCommandf(CMD_LIGHT0_VEC_X+3*light, params[0]);
		sendCommandf(CMD_LIGHT0_VEC_Y+3*light, params[1]);
		sendCommandf(CMD_LIGHT0_VEC_Z+3*light, params[2]);
		break;
	case GL_SPOT_EXPONENT:
		sendCommandf(CMD_LIGHT0_SPOT_EXP+light, params[0]);
		break;
	case GL_SPOT_CUTOFF:
		c->lights.spotlight[4] = (params[0] != 180.);
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

	set_light_type(light);	/* update light types, if needed */
}


void glLightf (GLenum light, GLenum pname, GLfloat param)
{
	glLightfv(light, pname, &param);
}

