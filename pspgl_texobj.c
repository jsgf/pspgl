#include <stdlib.h>
#include <string.h>
#include "pspgl_texobj.h"


const
struct pspgl_texobj __pspgl_texobj_default = {
	.ge_texreg_160x201 = {
		0xa0000000, /* ge_reg 0xa0/160 */
		0xa1000000,
		0xa2000000,
		0xa3000000,
		0xa4000000,
		0xa5000000,
		0xa6000000,
		0xa7000000,
		0xa8000000,
		0xa9000000,
		0xaa000000,
		0xab000000,
		0xac000000,
		0xad000000,
		0xae000000,
		0xaf000000,
		0xb0000000,
		0xb1000000,
		0xb2000000,
		0xb3000000,
		0xb4000000,
		0xb5000000,
		0xb6000000,
		0xb7000000,
		0xb8000000,
		0xb9000000,
		0xba000000,
		0xbb000000,
		0xbc000000,
		0xbd000000,
		0xbe000000,
		0xbf000000,
		0xc0000000,
		0xc1000000,
		0xc2000000,
		0xc3000000,
		0xc4000000,
		0xc5000000,
		0xc6000000,
		0xc7000000,
		0xc8000000,
		0xc9000000  /* ge_reg 0xc9/201 */
	},
	.priority = 0.5,
	.target = GL_TEXTURE_2D
};


struct pspgl_texobj* __pspgl_texobj_new (void)
{
	struct pspgl_texobj *t = malloc(sizeof(struct pspgl_texobj));

	if (t)
		memcpy(t, &__pspgl_texobj_default, sizeof(struct pspgl_texobj));

	return t;
}


void __pspgl_texobj_free (struct pspgl_texobj *t)
{
	free(t);
}

