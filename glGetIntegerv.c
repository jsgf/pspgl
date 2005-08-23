#include "pspgl_internal.h"

#define GU_PSM_5650		(0) /* Display, Texture, Palette */
#define GU_PSM_5551		(1) /* Display, Texture, Palette */
#define GU_PSM_4444		(2) /* Display, Texture, Palette */
#define GU_PSM_8888		(3) /* Display, Texture, Palette */

static const
struct {
	unsigned int red_bits : 4;
	unsigned int green_bits : 4;
	unsigned int blue_bits : 4;
	unsigned int alpha_bits : 4;
} colorfmt [] = {
	{ .red_bits = 5, .green_bits = 6, .blue_bits = 5, .alpha_bits = 0 },
	{ .red_bits = 5, .green_bits = 5, .blue_bits = 5, .alpha_bits = 1 },
	{ .red_bits = 4, .green_bits = 4, .blue_bits = 4, .alpha_bits = 4 },
	{ .red_bits = 8, .green_bits = 8, .blue_bits = 8, .alpha_bits = 8 },
};


void glGetIntegerv (GLenum pname, GLint *params)
{
	struct pspgl_surface *s = pspgl_curctx->draw;

	switch (pname) {
	case GL_MAX_TEXTURE_UNITS_ARB:
		*params = 1;
		break;
	case GL_MAX_TEXTURE_SIZE:
		*params = 256;		/* XXX CHECKME!! */
		break;
	case GL_VIEWPORT:
		/* XXX IMPRPOVE: better read from registers / register cache */
		params[0] = pspgl_curctx->viewport.x;
		params[1] = pspgl_curctx->viewport.y;
		params[2] = pspgl_curctx->viewport.width;
		params[3] = pspgl_curctx->viewport.height;
		break;
	case GL_DEPTH_BITS:
		*params = (s->depth_buffer == NULL) ? 0 : 16;
		break;
	case GL_STENCIL_BITS:
		*params = colorfmt[s->pixfmt].alpha_bits;
		break;
	case GL_RED_BITS:
		*params = colorfmt[s->pixfmt].red_bits;
		break;
	case GL_GREEN_BITS:
		*params = colorfmt[s->pixfmt].green_bits;
		break;
	case GL_BLUE_BITS:
		*params = colorfmt[s->pixfmt].blue_bits;
		break;
	case GL_ALPHA_BITS:
		*params = colorfmt[s->pixfmt].alpha_bits;
		break;
	default:
		;
	}
}

