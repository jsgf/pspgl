#include <string.h>
#include <pspge.h>

#include "pspgl_internal.h"

/* This is the set of registers that must be initialized before using the GE.
   The table was taken from sceGuInit() in PSPSDK.  To ensure compatibility
   with all PSP firmware revisions, this list should be sent as is, and before
   any other GE commands are sent.  Our specific GE initialization register
   table is further down. - MRB */
static const
uint32_t ge_init_list[] __attribute__((aligned(16))) =
{
	0x01000000, 0x02000000, 0x10000000, 0x12000000, 0x13000000, 0x15000000, 0x16000000, 0x17000000,
	0x18000000, 0x19000000, 0x1a000000, 0x1b000000, 0x1c000000, 0x1d000000, 0x1e000000, 0x1f000000,
	0x20000000, 0x21000000, 0x22000000, 0x23000000, 0x24000000, 0x25000000, 0x26000000, 0x27000000,
	0x28000000, 0x2a000000, 0x2b000000, 0x2c000000, 0x2d000000, 0x2e000000, 0x2f000000, 0x30000000,
	0x31000000, 0x32000000, 0x33000000, 0x36000000, 0x37000000, 0x38000000, 0x3a000000, 0x3b000000,
	0x3c000000, 0x3d000000, 0x3e000000, 0x3f000000, 0x40000000, 0x41000000, 0x42000000, 0x43000000,
	0x44000000, 0x45000000, 0x46000000, 0x47000000, 0x48000000, 0x49000000, 0x4a000000, 0x4b000000,
	0x4c000000, 0x4d000000, 0x50000000, 0x51000000, 0x53000000, 0x54000000, 0x55000000, 0x56000000,
	0x57000000, 0x58000000, 0x5b000000, 0x5c000000, 0x5d000000, 0x5e000000, 0x5f000000, 0x60000000,
	0x61000000, 0x62000000, 0x63000000, 0x64000000, 0x65000000, 0x66000000, 0x67000000, 0x68000000,
	0x69000000, 0x6a000000, 0x6b000000, 0x6c000000, 0x6d000000, 0x6e000000, 0x6f000000, 0x70000000,
	0x71000000, 0x72000000, 0x73000000, 0x74000000, 0x75000000, 0x76000000, 0x77000000, 0x78000000,
	0x79000000, 0x7a000000, 0x7b000000, 0x7c000000, 0x7d000000, 0x7e000000, 0x7f000000, 0x80000000,
	0x81000000, 0x82000000, 0x83000000, 0x84000000, 0x85000000, 0x86000000, 0x87000000, 0x88000000,
	0x89000000, 0x8a000000, 0x8b000000, 0x8c000000, 0x8d000000, 0x8e000000, 0x8f000000, 0x90000000,
	0x91000000, 0x92000000, 0x93000000, 0x94000000, 0x95000000, 0x96000000, 0x97000000, 0x98000000,
	0x99000000, 0x9a000000, 0x9b000000, 0x9c000000, 0x9d000000, 0x9e000000, 0x9f000000, 0xa0000000,
	0xa1000000, 0xa2000000, 0xa3000000, 0xa4000000, 0xa5000000, 0xa6000000, 0xa7000000, 0xa8040004,
	0xa9000000, 0xaa000000, 0xab000000, 0xac000000, 0xad000000, 0xae000000, 0xaf000000, 0xb0000000,
	0xb1000000, 0xb2000000, 0xb3000000, 0xb4000000, 0xb5000000, 0xb8000101, 0xb9000000, 0xba000000,
	0xbb000000, 0xbc000000, 0xbd000000, 0xbe000000, 0xbf000000, 0xc0000000, 0xc1000000, 0xc2000000,
	0xc3000000, 0xc4000000, 0xc5000000, 0xc6000000, 0xc7000000, 0xc8000000, 0xc9000000, 0xca000000,
	0xcb000000, 0xcc000000, 0xcd000000, 0xce000000, 0xcf000000, 0xd0000000, 0xd2000000, 0xd3000000,
	0xd4000000, 0xd5000000, 0xd6000000, 0xd7000000, 0xd8000000, 0xd9000000, 0xda000000, 0xdb000000,
	0xdc000000, 0xdd000000, 0xde000000, 0xdf000000, 0xe0000000, 0xe1000000, 0xe2000000, 0xe3000000,
	0xe4000000, 0xe5000000, 0xe6000000, 0xe7000000, 0xe8000000, 0xe9000000, 0xeb000000, 0xec000000,
	0xee000000, 0xf0000000, 0xf1000000, 0xf2000000, 0xf3000000, 0xf4000000, 0xf5000000, 0xf6000000,
	0xf7000000, 0xf8000000, 0xf9000000,
	0x0f000000, 0x0c000000, 0, 0
};

/* Convert a pointer into a physical RAM address. */
#define _PHYSADDR(x) ((void *) ((uint32_t) (x) & 0x1fffffff))

/* This sets the inital value of the register set.  It initializes
   every useful value register (it does not touch registers which
   trigger an action like drawing a primitive), because its also used
   to build the set of registers which need to be updated on a context
   switch.  These entries do not include the register number - the 8
   MSB are set to non-zero for defined entries, and the index itself
   is the command number.  */
#define INIT(c, v)		[c]         = ((1 << 24) | ((v) & 0xffffff))
#define INITRANGE(s,e,v)	[(s)...(e)] = ((1 << 24) | ((v) & 0xffffff))
#define UNINIT(c)		[c]	    = 0

#define FP_1_0			0x3f8000 /* 1.0 in PSP 24-bit FP */

static const
uint32_t ge_init_state[256] =
{
	INIT(CMD_VERTEXPTR,	0),
	INIT(CMD_INDEXPTR,	0),
	INIT(CMD_BASE,		0),
	INIT(CMD_VERTEXTYPE,	0),

	INIT(CMD_OFFSET_ADDR,	0),

	INIT(CMD_REGION1,	0),
	INIT(CMD_REGION2,	0),

	INITRANGE(CMD_ENA_LIGHTING, CMD_ENA_LOGIC, 0),
	INIT(CMD_ENA_DITHER,	1),
	INIT(CMD_ENA_CLIP,	1),

	INITRANGE(CMD_MORPH_WEIGHT0, CMD_MORPH_WEIGHT7, 0),

	INIT(CMD_PATCH_SUBDIV,	0x404),
	INIT(CMD_PATCH_PRIM,	0),
	INIT(CMD_PATCH_CULL,	0),

	INITRANGE(CMD_VIEWPORT_SX, CMD_VIEWPORT_TZ, 0),

	INIT(CMD_TEXTURE_SU,	FP_1_0),
	INIT(CMD_TEXTURE_SV,	FP_1_0),
	INIT(CMD_TEXTURE_TU,	0),
	INIT(CMD_TEXTURE_TV,	0),

	INIT(CMD_OFFSETX,	0),
	INIT(CMD_OFFSETY,	0),

	INIT(CMD_SHADEMODEL,	0),
	INIT(CMD_REV_NORM,	0),

	INIT(CMD_MATERIAL,		0),
	INIT(CMD_MATERIAL_EMS_C,	0x000000), /* emissive is 0,0,0,1 */
	INIT(CMD_MATERIAL_AMB_C,	0xffffff), /* ambient  is 1,1,1,1 */
	INIT(CMD_MATERIAL_DIF_C,	0xcccccc), /* diffuse  is .8,.8,.8,1 */
	INIT(CMD_MATERIAL_SPC_C,	0x000000), /* specular is 0,0,0,1 */
	INIT(CMD_MATERIAL_AMB_A,	0x0000ff), /* ambient alpha is 1 */
	INIT(CMD_MATERIAL_SPEC_POW,	FP_1_0),

	INIT(CMD_AMBIENT_COLOR,		0),
	INIT(CMD_AMBIENT_ALPHA,		0),

	INIT(CMD_LIGHTMODEL,		0),
	INITRANGE(CMD_LIGHT0_TYPE, CMD_LIGHT3_SPC_COL, 0),

	INIT(CMD_LIGHT0_POS_Z,		FP_1_0),
	INIT(CMD_LIGHT1_POS_Z,		FP_1_0),
	INIT(CMD_LIGHT2_POS_Z,		FP_1_0),
	INIT(CMD_LIGHT3_POS_Z,		FP_1_0),

	INIT(CMD_LIGHT0_VEC_Z,		FP_1_0),
	INIT(CMD_LIGHT1_VEC_Z,		FP_1_0),
	INIT(CMD_LIGHT2_VEC_Z,		FP_1_0),
	INIT(CMD_LIGHT3_VEC_Z,		FP_1_0),

	INIT(CMD_LIGHT0_ATT_CONST,	FP_1_0),
	INIT(CMD_LIGHT1_ATT_CONST,	FP_1_0),
	INIT(CMD_LIGHT2_ATT_CONST,	FP_1_0),
	INIT(CMD_LIGHT3_ATT_CONST,	FP_1_0),

	INIT(CMD_LIGHT0_AMB_COL,	0x000000),
	INIT(CMD_LIGHT0_DIF_COL,	0xffffff),
	INIT(CMD_LIGHT0_SPC_COL,	0xffffff),

	INIT(CMD_CULL_FACE,	0),

	INIT(CMD_DRAWBUF,	0),
	INIT(CMD_DRAWBUFWIDTH,	0),
	INIT(CMD_DEPTHBUF,	0),
	INIT(CMD_DEPTHBUFWIDTH,	0),

	INITRANGE(TEXSTATE_START, TEXSTATE_END, 0),
	INIT(CMD_TEX_MIPMAP0,	0x000000), /* 1.0 PSP firmware workaround? */
	INIT(CMD_TEX_STRIDE0,	0x040004),
	INIT(CMD_TEX_SIZE0,	0x000101),
	UNINIT(182),
	UNINIT(183),

	INIT(CMD_TEXENV_FUNC,	0),
	INIT(CMD_TEXENV_COL,	0),
	INIT(CMD_TEXCACHE_SYNC,		0),
	INIT(CMD_TEXCACHE_FLUSH,	0),

	INIT(CMD_FOG_FAR,	0),
	INIT(CMD_FOG_NEAR,	0),
	INIT(CMD_FOG_COLOR,	0),

	INIT(CMD_TEXSLOPE,	0),

	INIT(CMD_PSM,		0),
	INIT(CMD_CLEARMODE,	0),

	INIT(CMD_SCISSOR1,	0),
	INIT(CMD_SCISSOR2,	0),

	INIT(CMD_CLIP_NEAR,	0),
	INIT(CMD_CLIP_FAR,	0),

	INITRANGE(CMD_COLOR_FUNC, CMD_FIXEDCOL_DST, 0),
	INIT(CMD_DEPTH_FUNC,	GE_GEQUAL), /* GL_LESS -> GE_GEQUAL */

	INIT(CMD_DITHER0,	0x001d0c),
	INIT(CMD_DITHER1,	0x00f3e2),
	INIT(CMD_DITHER2,	0x000c1d),
	INIT(CMD_DITHER3,	0x00e2f3),

	INIT(CMD_LOGICOP,	0),

	INIT(CMD_DEPTH_MASK,	0),
	INIT(CMD_RGB_MASK,	0),
	INIT(CMD_ALPHA_MASK,	0),

	INIT(CMD_COPY_SRC_XY,	0),
	INIT(CMD_COPY_DST_XY,	0),
	INIT(CMD_COPY_SIZE,	0),

	INITRANGE(0xf0, 0xf9, 0), /* sceGuInit does this */
};

void __pspgl_ge_init (struct pspgl_context *c)
{
	int i;

	sceGeListEnQueue(_PHYSADDR(ge_init_list), NULL, 0, NULL);
	sceGeListSync(0, PSP_GE_LIST_DONE); 

	for (i=0; i<sizeof(ge_init_state)/sizeof(ge_init_state[0]); i++) {
		unsigned long val = ge_init_state[i];

		if (val & 0xff000000) {
			__pspgl_context_writereg(c, i, val);
			__pspgl_context_register[i / 32] |= 1 << (1 % 32);
		}
	}

	glScissor(0, 0, c->draw->width, c->draw->height);
	glViewport(0, 0, c->draw->width, c->draw->height);
	glDepthRange(0.0, 1.0);
	c->clear.depth = 0;	/* = 1.0f in OpenGL coord system */
	c->viewport.depth_offset = 0.0;
	c->swap_interval = 1;

	/* Material ambient and current color share the same hardware
	   register, so we need to keep separate copies of the state;
	   the other material colors are set up with all the other
	   hardware state */
	c->current.color = 0xffffffff;		/* default color is 1,1,1,1 */
	c->material.ambient = 0xff333333;	/* material ambient color is .2, .2, .2, 1 */

	/* make sure all the stencil/alpha mask state is set up right */
	glStencilMask(~0);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDisable(GL_STENCIL_TEST);
}

