#include "pspgl_internal.h"


void glColorMask (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
	unsigned long mask = red ? 0x000000ff : 0;

	if (green)
		mask |= 0x0000ff00;

	if (blue)
		mask |= 0x00ff0000;

        sendCommandi(232, mask);
        sendCommandi(233, alpha ? 0xff : 0);
}
