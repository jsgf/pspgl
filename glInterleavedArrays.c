#include "pspgl_internal.h"


struct {
	unsigned int enable_texcoord : 1;
	unsigned int enable_color : 1;
	unsigned int enable_normal : 1;
	unsigned int size_texcoord : 3;
	unsigned int size_color : 3;
	unsigned int size_vertex : 3;

	unsigned int color_is_float : 1;

	unsigned int offset_color : 3;
	unsigned int offset_normal: 4;
	unsigned int offset_vertex : 4;
	unsigned int stride : 4;
} array_desc [] = {
	{ 0, 0, 0,	0, 0, 2,	0,	0, 0, 0, 2 },	/* GL_V2F */
	{ 0, 0, 0,	0, 0, 3,	0,	0, 0, 0, 3 },	/* GL_V3F */
	{ 0, 1, 0,	0, 4, 2,	0,	0, 0, 1, 3 },	/* GL_C4UB_V2F */
	{ 0, 1, 0,	0, 4, 3,	0,	0, 0, 1, 4 },	/* GL_C4UB_V3F */

	{ 0, 1, 0,	0, 3, 3,	1,	0, 0, 3, 6 },	/* GL_C3F_V3F */
	{ 0, 0, 1,	0, 0, 3,	0,	0, 0, 3, 6 },	/* GL_N3F_V3F */
	{ 0, 1, 1,	0, 4, 3,	1,	0, 4, 7, 10 },	/* GL_C4F_N3F_V3F */
	{ 1, 0, 0,	2, 0, 3,	0,	0, 0, 2, 5 },	/* GL_T2F_V3F */

	{ 1, 0, 0,	4, 0, 4,	0,	0, 0, 4, 8 },	/* GL_T4F_V4F */
	{ 1, 1, 0,	2, 4, 3,	0,	2, 0, 3, 6 },	/* GL_T2F_C4UB_V3F */
	{ 1, 1, 0,	2, 3, 3,	1,	2, 0, 5, 8 },	/* GL_T2F_C3F_V3F */
	{ 1, 0, 1,	2, 0, 3,	0,	0, 2, 5, 8 },	/* GL_T2F_N3F_V3F */

	{ 1, 1, 1,	2, 4, 3,	1,	2, 6, 9, 12 },	/* GL_T2F_C4F_N3F_V3F */
	{ 1, 1, 1,	4, 4, 4,	1,	4, 8, 11, 15 }	/* GL_T4F_C4F_N3F_V4F */
};


void glInterleavedArrays (GLenum format, GLsizei stride, const GLvoid *pointer)
{
	unsigned long idx = format - GL_V2F;
	unsigned long p = (unsigned long) pointer;

	if (idx > sizeof(array_desc)/sizeof(array_desc[0])) {
		GLERROR(GL_INVALID_ENUM);
		return;
	}

	pspgl_curctx->vertex_array.vertex.enabled = 1;
	pspgl_curctx->vertex_array.texcoord.enabled = array_desc[idx].enable_texcoord;
	pspgl_curctx->vertex_array.color.enabled = array_desc[idx].enable_color;
	pspgl_curctx->vertex_array.normal.enabled = array_desc[idx].enable_normal;

	pspgl_curctx->vertex_array.vertex.size = array_desc[idx].size_vertex;
	pspgl_curctx->vertex_array.texcoord.size = array_desc[idx].size_texcoord;
	pspgl_curctx->vertex_array.color.size = array_desc[idx].size_color;

	pspgl_curctx->vertex_array.vertex.type = GL_FLOAT;
	pspgl_curctx->vertex_array.texcoord.type = GL_FLOAT;
	pspgl_curctx->vertex_array.color.type = array_desc[idx].color_is_float ? GL_FLOAT : GL_UNSIGNED_BYTE;
	pspgl_curctx->vertex_array.normal.type = GL_FLOAT;

	if (stride == 0)
		stride = 4 * array_desc[idx].stride;

	pspgl_curctx->vertex_array.vertex.stride = stride;
	pspgl_curctx->vertex_array.texcoord.stride = stride;
	pspgl_curctx->vertex_array.color.stride = stride;
	pspgl_curctx->vertex_array.normal.stride = stride;

	pspgl_curctx->vertex_array.vertex.ptr = (void *) (p + 4 * array_desc[idx].offset_vertex);
	pspgl_curctx->vertex_array.normal.ptr = (void *) (p + 4 * array_desc[idx].offset_normal);
	pspgl_curctx->vertex_array.color.ptr = (void *) (p + 4 * array_desc[idx].offset_color);
	pspgl_curctx->vertex_array.texcoord.ptr = (void *) (p);
}
