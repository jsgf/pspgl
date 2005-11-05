#ifndef _guconsts_h_
#define _guconsts_h_

/*
   This file contains definitions used by the PSP's GU hardware.
   It is API-independent.
 */

/* symbolic defines for GU commands */

#define CMD_VERTEXPTR		1
#define CMD_INDEXPTR		2

#define CMD_PRIM		4

#define CMD_BASE		16

#define CMD_VERTEXTYPE		18

#define CMD_REGION1		21
#define CMD_REGION2		22
#define CMD_ENA_LIGHTING	23
#define CMD_ENA_LIGHT0		24
#define CMD_ENA_LIGHT1		25
#define CMD_ENA_LIGHT2		26
#define CMD_ENA_LIGHT3		27
#define CMD_ENA_CLIP		28
#define CMD_ENA_CULL		29
#define CMD_ENA_TEXTURE		30
#define CMD_ENA_FOG		31
#define CMD_ENA_DITHER		32
#define CMD_ENA_BLEND		33
#define CMD_ENA_ALPHA_TEST	34
#define CMD_ENA_DEPTH_TEST	35
#define CMD_ENA_STENCIL_TEST	36
#define CMD_ENA_ANTIALIAS	37
#define CMD_ENA_LOGIC		40

#define CMD_MAT_MODEL_TRIGGER	58
#define CMD_MAT_MODEL_LOAD	59
#define CMD_MAT_VIEW_TRIGGER	60
#define CMD_MAT_VIEW_LOAD	61
#define CMD_MAT_PROJ_TRIGGER	62
#define CMD_MAT_PROJ_LOAD	63
#define CMD_MAT_TEXTURE_TRIGGER	64
#define CMD_MAT_TEXTURE_LOAD	65

#define CMD_VIEWPORT_SX		66
#define CMD_VIEWPORT_SY		67
#define CMD_VIEWPORT_SZ		68
#define CMD_VIEWPORT_TX		69
#define CMD_VIEWPORT_TY		70
#define CMD_VIEWPORT_TZ		71

#define CMD_OFFSETX		76
#define CMD_OFFSETY		77

#define CMD_SHADEMODEL		80

#define CMD_MATERIAL		83
#define CMD_MATERIAL_EMS_C	84
#define CMD_MATERIAL_AMB_C	85
#define CMD_MATERIAL_DIF_C	86
#define CMD_MATERIAL_SPC_C	87
#define CMD_MATERIAL_AMB_A	88
#define CMD_MATERIAL_SPEC_POW	91

#define CMD_AMBIENT_COLOR	92
#define CMD_AMBIENT_ALPHA	93
#define CMD_LIGHTMODEL		94
#define CMD_LIGHT0_TYPE		95
#define CMD_LIGHT1_TYPE		96
#define CMD_LIGHT2_TYPE		97
#define CMD_LIGHT3_TYPE		98
#define CMD_LIGHT0_POS_X	99
#define CMD_LIGHT0_POS_Y	100
#define CMD_LIGHT0_POS_Z	101
#define CMD_LIGHT1_POS_X	102
#define CMD_LIGHT1_POS_Y	103
#define CMD_LIGHT1_POS_Z	104
#define CMD_LIGHT2_POS_X	105
#define CMD_LIGHT2_POS_Y	106
#define CMD_LIGHT2_POS_Z	107
#define CMD_LIGHT3_POS_X	108
#define CMD_LIGHT3_POS_Y	109
#define CMD_LIGHT3_POS_Z	110
#define CMD_LIGHT0_VEC_X	111
#define CMD_LIGHT0_VEC_Y	112
#define CMD_LIGHT0_VEC_Z	113
#define CMD_LIGHT1_VEC_X	114
#define CMD_LIGHT1_VEC_Y	115
#define CMD_LIGHT1_VEC_Z	116
#define CMD_LIGHT2_VEC_X	117
#define CMD_LIGHT2_VEC_Y	118
#define CMD_LIGHT2_VEC_Z	119
#define CMD_LIGHT3_VEC_X	120
#define CMD_LIGHT3_VEC_Y	121
#define CMD_LIGHT3_VEC_Z	122
#define CMD_LIGHT0_ATT_CONST	123
#define CMD_LIGHT0_ATT_LINEAR	124
#define CMD_LIGHT0_ATT_QUAD	125
#define CMD_LIGHT1_ATT_CONST	126
#define CMD_LIGHT1_ATT_LINEAR	127
#define CMD_LIGHT1_ATT_QUAD	128
#define CMD_LIGHT2_ATT_CONST	129
#define CMD_LIGHT2_ATT_LINEAR	130
#define CMD_LIGHT2_ATT_QUAD	131
#define CMD_LIGHT3_ATT_CONST	132
#define CMD_LIGHT3_ATT_LINEAR	133
#define CMD_LIGHT3_ATT_QUAD	134
#define CMD_LIGHT0_CUTOFF	135
#define CMD_LIGHT1_CUTOFF	136
#define CMD_LIGHT2_CUTOFF	137
#define CMD_LIGHT3_CUTOFF	138
#define CMD_LIGHT0_SPOT_EXP	139
#define CMD_LIGHT1_SPOT_EXP	140
#define CMD_LIGHT2_SPOT_EXP	141
#define CMD_LIGHT3_SPOT_EXP	142
#define CMD_LIGHT0_AMB_COL	143
#define CMD_LIGHT0_DIF_COL	144
#define CMD_LIGHT0_SPC_COL	145
#define CMD_LIGHT1_AMB_COL	146
#define CMD_LIGHT1_DIF_COL	147
#define CMD_LIGHT1_SPC_COL	148
#define CMD_LIGHT2_AMB_COL	149
#define CMD_LIGHT2_DIF_COL	150
#define CMD_LIGHT2_SPC_COL	151
#define CMD_LIGHT3_AMB_COL	152
#define CMD_LIGHT3_DIF_COL	153
#define CMD_LIGHT3_SPC_COL	154

#define CMD_CULL_FACE		155 /* 0=CCW 1=CW */
#define CMD_DRAWBUF		156
#define CMD_DRAWBUFWIDTH	157
#define CMD_DEPTHBUF		158
#define CMD_DEPTHBUFWIDTH	159

#define TEXSTATE_START		CMD_TEX_MIPMAP0

#define CMD_TEX_MIPMAP0		160
#define CMD_TEX_MIPMAP1		161
#define CMD_TEX_MIPMAP2		162
#define CMD_TEX_MIPMAP3		163
#define CMD_TEX_MIPMAP4		164
#define CMD_TEX_MIPMAP5		165
#define CMD_TEX_MIPMAP6		166
#define CMD_TEX_MIPMAP7		167

#define CMD_TEX_STRIDE0		168
#define CMD_TEX_STRIDE1		169
#define CMD_TEX_STRIDE2		170
#define CMD_TEX_STRIDE3		171
#define CMD_TEX_STRIDE4		172
#define CMD_TEX_STRIDE5		173
#define CMD_TEX_STRIDE6		174
#define CMD_TEX_STRIDE7		175

#define CMD_SET_CLUT		176
#define CMD_SET_CLUT_MSB	177

#define CMD_TEX_SIZE0		184
#define CMD_TEX_SIZE1		185
#define CMD_TEX_SIZE2		186
#define CMD_TEX_SIZE3		187
#define CMD_TEX_SIZE4		188
#define CMD_TEX_SIZE5		189
#define CMD_TEX_SIZE6		190
#define CMD_TEX_SIZE7		191

#define CMD_TEXMODE		194
#define CMD_TEXFMT		195

#define CMD_CLUT_BLKS		196
#define CMD_CLUT_MODE		197

#define CMD_TEXFILT		198
#define CMD_TEXWRAP		199

#define TEXSTATE_END		CMD_TEXWRAP

#define CMD_TEXENV_FUNC		201
#define CMD_TEXENV_COL		202
#define CMD_TEXCACHE_FLUSH	203
#define CMD_TEXCACHE_SYNC	204
#define CMD_FOG_FAR		205
#define CMD_FOG_NEAR		206
#define CMD_FOG_COLOR		206
#define CMD_PSM			210
#define CMD_CLEARMODE		211
#define CMD_SCISSOR1		212
#define CMD_SCISSOR2		213
#define CMD_CLIP_NEAR		214
#define CMD_CLIP_FAR		215

#define CMD_ALPHA_FUNC		219
#define CMD_STENCIL_FUNC	220
#define CMD_STENCIL_OP		221
#define CMD_DEPTH_FUNC		222
#define CMD_BLEND_FUNC		223
#define CMD_FIXEDCOL_SRC	224
#define CMD_FIXEDCOL_DST	225

#define CMD_LOGICOP		230
#define CMD_DEPTH_MASK		231
#define CMD_RGB_MASK		232
#define CMD_ALPHA_MASK		233
/*
   The definitions below come from pspgu.h
 */

/* Primitive types */
#define GU_POINTS		(0)
#define GU_LINES		(1)
#define GU_LINE_STRIP		(2)
#define GU_TRIANGLES		(3)
#define GU_TRIANGLE_STRIP	(4)
#define GU_TRIANGLE_FAN		(5)
#define GU_SPRITES		(6)

/* Vertex Declarations Begin */
#define GU_TEXTURE_SHIFT(n)	((n)<<0)
#define GU_TEXTURE_8BIT		GU_TEXTURE_SHIFT(1)
#define GU_TEXTURE_16BIT	GU_TEXTURE_SHIFT(2)
#define GU_TEXTURE_32BITF	GU_TEXTURE_SHIFT(3)

#define GU_COLOR_SHIFT(n)	((n)<<2)
#define GU_COLOR_RES1		GU_COLOR_SHIFT(1)
#define GU_COLOR_RES2		GU_COLOR_SHIFT(2)
#define GU_COLOR_RES3		GU_COLOR_SHIFT(3)
#define GU_COLOR_5650		GU_COLOR_SHIFT(4)
#define GU_COLOR_5551		GU_COLOR_SHIFT(5)
#define GU_COLOR_4444		GU_COLOR_SHIFT(6)
#define GU_COLOR_8888		GU_COLOR_SHIFT(7)

#define GU_NORMAL_SHIFT(n)	((n)<<5)
#define GU_NORMAL_8BIT		GU_NORMAL_SHIFT(1)
#define GU_NORMAL_16BIT		GU_NORMAL_SHIFT(2)
#define GU_NORMAL_32BITF	GU_NORMAL_SHIFT(3)

#define GU_VERTEX_SHIFT(n)	((n)<<7)
#define GU_VERTEX_8BIT		GU_VERTEX_SHIFT(1)
#define GU_VERTEX_16BIT		GU_VERTEX_SHIFT(2)
#define GU_VERTEX_32BITF	GU_VERTEX_SHIFT(3)

#define GU_WEIGHT_SHIFT(n)	((n)<<9)
#define GU_WEIGHT_8BIT		GU_WEIGHT_SHIFT(1)
#define GU_WEIGHT_16BIT		GU_WEIGHT_SHIFT(2)
#define GU_WEIGHT_32BITF	GU_WEIGHT_SHIFT(3)

#define GU_INDEX_SHIFT(n)	((n)<<11)
#define GU_INDEX_8BIT		GU_INDEX_SHIFT(1)
#define GU_INDEX_16BIT		GU_INDEX_SHIFT(2)
#define GU_INDEX_RES3		GU_INDEX_SHIFT(3)

#define GU_WEIGHTS(n)		((((n)-1)&7)<<14)
#define GU_VERTICES(n)		((((n)-1)&7)<<18)

#define GU_TRANSFORM_SHIFT(n)	((n)<<23)
#define GU_TRANSFORM_3D		GU_TRANSFORM_SHIFT(0)
#define GU_TRANSFORM_2D		GU_TRANSFORM_SHIFT(1)
/* Vertex Declarations End */

/* Pixel Formats */
#define GE_RGB_565		0 /* display, texture, palette */
#define GE_RGBA_5551		1 /* display, texture, palette */
#define GE_RGBA_4444		2 /* display, texture, palette */
#define GE_RGBA_8888		3 /* display, texture, palette */
#define GE_INDEX_4BIT		4 /* texture */
#define GE_INDEX_8BIT		5 /* texture */
#define GE_INDEX_16BIT		6 /* texture */
#define GE_INDEX_32BIT		7 /* texture */
#define GE_DXT1			8 /* texture */
#define GE_DXT3			9 /* texture */
#define GE_DXT5			10 /* texture */

/* Shading Model */
#define GU_FLAT			(0)
#define GU_SMOOTH		(1)

/* Logical operation */
#define GU_CLEAR		(0)
#define GU_AND			(1)
#define GU_AND_REVERSE		(2)
#define GU_COPY			(3)
#define GU_AND_INVERTED		(4)
#define GU_NOOP			(5)
#define GU_XOR			(6)
#define GU_OR			(7)
#define GU_NOR			(8)
#define GU_EQUIV		(9)
#define GU_INVERTED		(10)
#define GU_OR_REVERSE		(11)
#define GU_COPY_INVERTED	(12)
#define GU_OR_INVERTED		(13)
#define GU_NAND			(14)
#define GU_SET			(15)

#define GE_TEX_WRAP_REPEAT	0
#define GE_TEX_WRAP_CLAMP	1

/* Texture Filter */
#define GE_TEX_FILTER_NEAREST			0
#define GE_TEX_FILTER_LINEAR			1
#define GE_TEX_FILTER_NEAREST_MIPMAP_NEAREST	4
#define GE_TEX_FILTER_LINEAR_MIPMAP_NEAREST	5
#define GE_TEX_FILTER_NEAREST_MIPMAP_LINEAR	6
#define GE_TEX_FILTER_LINEAR_MIPMAP_LINEAR	7

/* Texture Map Mode */
#define GU_TEXTURE_COORDS	(0)
#define GU_TEXTURE_MATRIX	(1)
#define GU_ENVIRONMENT_MAP	(2)

/* Texture Projection Map Mode */
#define GU_POSITION		(0)
#define GU_UV			(1)
#define GU_NORMALIZED_NORMAL	(2)
#define GU_NORMAL		(3)

/* Front Face Direction */
#define GU_CW			(0)
#define GU_CCW			(1)

/* Test Function */
#define GU_NEVER		(0)
#define GU_ALWAYS		(1)
#define GU_EQUAL		(2)
#define GU_NOTEQUAL		(3)
#define GU_LESS			(4)
#define GU_LEQUAL		(5)
#define GU_GREATER		(6)
#define GU_GEQUAL		(7)

/* Clear Buffer Mask */
#define GU_COLOR_BUFFER_BIT	(1)
#define GU_STENCIL_BUFFER_BIT	(2)
#define GU_DEPTH_BUFFER_BIT	(4)

/* Blending Op */
#define GU_ADD			(0)
#define GU_SUBTRACT		(1)

/* Blending Factor */
#define GU_SRC_COLOR		(0)
#define GU_ONE_MINUS_SRC_COLOR	(1)
#define GU_SRC_ALPHA		(2)
#define GU_ONE_MINUS_SRC_ALPHA	(3)
#define GU_DST_COLOR		(0)
#define GU_ONE_MINUS_DST_COLOR	(1)
#define GU_FIX			(10)

/* Stencil Operations */
#define GU_KEEP			(0)
#define GU_ZERO			(1)
#define GU_REPLACE		(2)
#define GU_INVERT		(3)
#define GU_INCR			(4)
#define GU_DECR			(5)

/* Light Components */
#define GU_AMBIENT		(1)
#define GU_DIFFUSE		(2)
#define GU_SPECULAR		(4)
#define GU_AMBIENT_AND_DIFFUSE	(GU_AMBIENT|GU_DIFFUSE)
#define GU_DIFFUSE_AND_SPECULAR	(GU_DIFFUSE|GU_SPECULAR)
#define GU_UNKNOWN_LIGHT_COMPONENT (8)

/* Light Type */
#define GU_DIRECTIONAL		(0)
#define GU_POINTLIGHT		(1)
#define GU_SPOTLIGHT		(2)

/* Texture environment */
#define GE_TEXENV_MODULATE				0
#define GE_TEXENV_DECAL					1
#define GE_TEXENV_BLEND					2
#define GE_TEXENV_REPLACE				3
#define GE_TEXENV_ADD					4

#define GE_TEXENV_RGB					(0<<8)
#define GE_TEXENV_RGBA					(1<<8)

#endif	/* _guconsts_h */
