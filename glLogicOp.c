#include "pspgl_internal.h"


void glLogicOp (GLenum opcode)
{
	if (opcode < GL_CLEAR || opcode > GL_SET) {
		GLERROR(GL_INVALID_ENUM);
	} else {
		opcode &= 0x000f;
		sendCommandi(230, opcode);
	}
}

