#include "pspgl_internal.h"

void pspgl_enable_state (GLenum cap, int enable)
{
	unsigned char opcode;

	switch (cap) {
	case GL_FOG:
		opcode = 31;
		break;
	case GL_LIGHTING:
		opcode = 23;
		break;
	case GL_TEXTURE_2D:
		pspgl_curctx->texture.enabled = enable;
		opcode = 30;
		break;
	case GL_CULL_FACE:
		opcode = 29;
		break;
	case GL_ALPHA_TEST:
		opcode = 34;
		break;
	case GL_BLEND:
		opcode = 33;
		break;
	case GL_COLOR_LOGIC_OP:
		opcode = 40;
		break;
	case GL_DITHER:
		opcode = 32;
		break;
	case GL_STENCIL_TEST:
		if (enable && pspgl_curctx->draw->stencil_buffer == 0) {
			GLERROR(GL_INVALID_OPERATION);
			enable = GL_FALSE;
		}
		opcode = 36;
		break;
	case GL_DEPTH_TEST:
		if (enable && pspgl_curctx->draw->depth_buffer == 0) {
			GLERROR(GL_INVALID_OPERATION);
			enable = GL_FALSE;
		}
		opcode = 35;
		break;
	case GL_LIGHT0:
	case GL_LIGHT1:
	case GL_LIGHT2:
	case GL_LIGHT3:
		opcode = cap - GL_LIGHT0 + 24;
		break;
	case GL_LINE_SMOOTH:
	case GL_POINT_SMOOTH:
		opcode = 37;	/* XXX : antialiasing. both line and point? */
		break;
	case GL_SCISSOR_TEST:
		pspgl_curctx->scissor_test.enabled = enable;
		if (enable) {
			glScissor(pspgl_curctx->scissor_test.x,
				  pspgl_curctx->scissor_test.y,
				  pspgl_curctx->scissor_test.width,
				  pspgl_curctx->scissor_test.height);
		} else {
			glScissor(pspgl_curctx->viewport.x,
				  pspgl_curctx->viewport.y,
				  pspgl_curctx->viewport.width,
				  pspgl_curctx->viewport.height);
		}
		return;
	case GL_COLOR_MATERIAL:
	case GL_NORMALIZE:
	case GL_RESCALE_NORMAL:
	case GL_POLYGON_OFFSET_FILL:
	case GL_VERTEX_ARRAY:
	case GL_NORMAL_ARRAY:
	case GL_COLOR_ARRAY:
	case GL_TEXTURE_COORD_ARRAY:
	case GL_MULTISAMPLE:
	case GL_SAMPLE_ALPHA_TO_COVERAGE:
	case GL_SAMPLE_ALPHA_TO_ONE:
	case GL_SAMPLE_COVERAGE:
	default:
		GLERROR(GL_INVALID_ENUM);
		return;
	}

	sendCommandi(opcode, enable);
}


void glEnable (GLenum cap)
{
	pspgl_enable_state(cap, 1);
}


void glDisable (GLenum cap)
{
	pspgl_enable_state(cap, 0);
}
