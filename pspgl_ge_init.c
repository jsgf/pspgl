#include <string.h>
#include "pspgl_internal.h"


/* list only non-zero defaults here... */
static const
unsigned long ge_init_state [] =
{

	0x01000000, 0x02000000, 0x10000000, 0x12000000, 0x13000000, 0x15000000, 0x16000000, 0x17000000,
	0x18000000, 0x19000000, 0x1a000000, 0x1b000000, /* 0x1c000001,*/ 0x1d000000, 0x1e000000, 0x1f000000,
	/*0x20000000,*/ 0x21000000, 0x22000000, 0x23000000, 0x24000000, 0x25000000, 0x26000000, 0x27000000,
	0x28000000, 0x2a000000, 0x2b000000, 0x2c000000, 0x2d000000, 0x2e000000, 0x2f000000, 0x30000000,
	0x31000000, 0x32000000, 0x33000000, /*0x36000000,*/ 0x37000000, 0x38000000, /*0x3a000000, 0x3b000000,
	0x3c000000, 0x3d000000, 0x3e000000, 0x3f000000, 0x40000000, 0x41000000,*/ 0x42000000, 0x43000000,
	0x44000000, 0x45000000, 0x46000000, 0x47000000, /*0x48000000, 0x49000000,*/ 0x4a000000, 0x4b000000,
	0x4c000000, 0x4d000000, 0x50000000, 0x51000000, 0x53000000, /*0x54000000, 0x55000000, 0x56000000,
	0x57000000, 0x58000000, 0x5b000000,*/ 0x5c000000, 0x5d000000, 0x5e000000, 0x5f000000, 0x60000000,
	0x61000000, 0x62000000, 0x63000000, 0x64000000, 0x65000000, 0x66000000, 0x67000000, 0x68000000,
	0x69000000, 0x6a000000, 0x6b000000, 0x6c000000, 0x6d000000, 0x6e000000, 0x6f000000, 0x70000000,
	0x71000000, 0x72000000, 0x73000000, 0x74000000, 0x75000000, 0x76000000, 0x77000000, 0x78000000,
	0x79000000, 0x7a000000, 0x7b000000, 0x7c000000, 0x7d000000, 0x7e000000, 0x7f000000, 0x80000000,
	0x81000000, 0x82000000, 0x83000000, 0x84000000, 0x85000000, 0x86000000, 0x87000000, 0x88000000,
	0x89000000, 0x8a000000, 0x8b000000, 0x8c000000, 0x8d000000, 0x8e000000, 0x8f000000, 0x90000000,
	0x91000000, 0x92000000, 0x93000000, 0x94000000, 0x95000000, 0x96000000, 0x97000000, 0x98000000,
	0x99000000, 0x9a000000, /*0x9b000000,*/ 0x9c000000, 0x9d000000, 0x9e000000, 0x9f000000, 0xa0000000,
	0xa1000000, 0xa2000000, 0xa3000000, 0xa4000000, 0xa5000000, 0xa6000000, 0xa7000000,

	/* ignore texobj regs, initialized by texobj_default */				 /* 0xa8040004,
	0xa9000000, 0xaa000000, 0xab000000, 0xac000000, 0xad000000, 0xae000000, 0xaf000000, 0xb0000000,
	0xb1000000, 0xb2000000, 0xb3000000, 0xb4000000,	0xb5000000, 0xb8000101, 0xb9000000, 0xba000000, 
	0xbb000000, 0xbc000000, 0xbd000000, 0xbe000000, 0xbf000000, 0xc0000000, 0xc1000000, 0xc2000000,
	0xc3000000, 0xc4000000, 0xc5000000, 0xc6000000, 0xc7000000, 0xc8000000, 0xc9000000, */

											 /* 0xca000000,
	0xcb000000, 0xcc000000, 0xcd000000, 0xce000000, 0xcf000000, 0xd0000000, 0xd2000000, 0xd3000000,
	0xd4000000, 0xd5000000, 0xd6000000, 0xd7000000, 0xd8000000, 0xd9000000, 0xda000000, 0xdb000000,
	0xdc000000, 0xdd000000, 0xde000000, 0xdf000000, 0xe0000000, 0xe1000000, 0xe2000000, 0xe3000000,
	0xe4000000, 0xe5000000, 0xe6000000, 0xe7000000, 0xe8000000, 0xe9000000, 0xeb000000, 0xec000000,
	0xee000000, 0xf0000000, 0xf1000000, 0xf2000000, 0xf3000000, 0xf4000000, 0xf5000000, 0xf6000000,
	0xf7000000, 0xf8000000, 0xf9000000,
*/
	0x1c000001,	/* Enable Frustum Clip Planes */
	0x20000001,	/* Enable Dithering */
	0x36000101,	/* Patch Divide, 1x1 */

	0x483f8000,	/* Texture Scale S 1.0 */
	0x493f8000,	/* Texture Scale T 1.0 */
	0x54000000,	/* emissive color 0.0, 0.0, 0.0, 1.0 */
	0x55333333,	/* ambient  color 0.2, 0.2, 0.2, 1.0 */
	0x56cccccc,	/* diffuse  color 0.8, 0.8, 0.8, 1.0 */
	0x57000000,	/* specular color 0.0, 0.0, 0.0, 1.0 */
	0x580000ff,	/* ambient alpha 1 */
	0x5b3f8000,	/* Specular Power = 1.0 */
	0x9b000001,	/* Front Face Orientation GL_CCW */
	0xc7000106,	/* Texfilter mag (LINEAR), min (NEAREST_MIPMAP_LINEAR) */
        0xe2001d0c,	/* Dither Matrix, Row 0 */
	0xe300f3e2,	/* Dither Matrix, Row 1 */
	0xe4000c1d,	/* Dither Matrix, Row 2 */
	0xe500e2f3,	/* Dither Matrix, Row 3 */
	0xde000004,	/* Depth Test Function GL_LESS */
};


static const
unsigned long ge_matrix_init_state [] = {
	0x3c000000,	/* View Matrix Select = 0 */
	0x3d3f8000,	/* View Matrix Upload 1.00000 */
	0x3d000000,	/* View Matrix Upload 0.00000 */
	0x3d000000,	/* View Matrix Upload 0.00000 */
	0x3d000000,	/* View Matrix Upload 0.00000 */
	0x3d3f8000,	/* View Matrix Upload 1.00000 */
	0x3d000000,	/* View Matrix Upload 0.00000 */
	0x3d000000,	/* View Matrix Upload 0.00000 */
	0x3d000000,	/* View Matrix Upload 0.00000 */
	0x3d3f8000,	/* View Matrix Upload 1.00000 */
	0x3d000000,	/* View Matrix Upload 0.00000 */
	0x3d000000,	/* View Matrix Upload 0.00000 */
	0x3d000000,	/* View Matrix Upload 0.00000 */
};



void pspgl_ge_init (struct pspgl_context *c)
{
	int i;

	for (i=0; i<sizeof(ge_init_state)/sizeof(ge_init_state[0]); i++)
		pspgl_context_writereg(c, ge_init_state[i] >> 24, ge_init_state[i]);

	/* matrix registers are overloaded, not cached. Use direct write-through. */
	for (i=0; i<sizeof(ge_matrix_init_state)/sizeof(ge_matrix_init_state[0]); i++)
		pspgl_dlist_enqueue_cmd(c->dlist_current, ge_matrix_init_state[i]);

	/* create & initialize new matrix stacks, matrix mode is GL_MODELVIEW when done... */
	for (i=GL_TEXTURE; --i>=GL_MODELVIEW; ) {
		glMatrixMode(i);
		glPushMatrix();
		glLoadIdentity();
	}

	memcpy(&c->texobj0, &pspgl_texobj_default, sizeof(c->texobj0));
	c->texobj_current = &c->texobj0;

	/* load register set of new texture object and mark all related registers as dirty*/
	memcpy(&c->ge_reg[160], c->texobj_current->ge_texreg_160x201, sizeof(c->texobj_current->ge_texreg_160x201) - 4);
	c->ge_reg_touched[4] |= 0xffffffff;
	c->ge_reg_touched[5] |= 0x000001ff;

	glScissor(0, 0, c->draw->width, c->draw->height);
	glViewport(0, 0, c->draw->width, c->draw->height);
	glDepthRange(0.0, 1.0);
	c->clear.depth = 1.0;
	c->depth_offset = 0.0;
	c->swap_interval = 1;
}

