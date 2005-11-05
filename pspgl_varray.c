#include <string.h>

#include "pspgl_internal.h"

unsigned __pspgl_gl_sizeof(GLenum type)
{
	switch(type) {
	case GL_BYTE:
	case GL_UNSIGNED_BYTE:
		return sizeof(GLubyte);

	case GL_SHORT:
	case GL_UNSIGNED_SHORT:
		return sizeof(GLushort);

	case GL_UNSIGNED_INT:
		return sizeof(GLuint);

	case GL_FLOAT:
		return sizeof(GLfloat);
	}

	return 0;
}

static
int glfmt2gefmt (GLenum type)
{
	int ge_type;

	switch (type) {
	case GL_BYTE:
		ge_type = GE_INT_8BIT;
		break;
	case GL_SHORT:
		ge_type = GE_INT_16BIT;
		break;
	case GL_FLOAT:
		ge_type = GE_FLOAT_32BIT;
		break;
	default:
		ge_type = -1;
	}

	return ge_type;
}

static unsigned ge_sizeof(unsigned type)
{
	switch(type) {
	case GE_INT_8BIT:	return 1;
	case GE_INT_16BIT:	return 2;
	case GE_FLOAT_32BIT:	return 4;
	default:		return 0;
	}
}

static void cvt_color_float3_ub(void *to, const void *from, const struct attrib *attr)
{
	const GLfloat *src = from;
	unsigned long *dest = to;
	unsigned long ret = COLOR3(src) | 0xff000000;

	//psp_log("(%g,%g,%g) -> %08x\n", src[0], src[1], src[2], ret);
	*dest = ret;
}

static void cvt_color_float4_ub(void *to, const void *from, const struct attrib *attr)
{
	const GLfloat *src = from;
	unsigned long *dest = to;
	unsigned long ret = COLOR4(src);

	//psp_log("(%g,%g,%g,%g) -> %08x\n", src[0], src[1], src[2], src[3], ret);
	*dest = ret;
}

static void cvt_color_ub3_ub(void *to, const void *from, const struct attrib *attr)
{
	const unsigned char *src = from;
	unsigned long *dest = to;

	*dest = 0xff000000 | (src[2] << 16) | (src[1] << 8) | src[0];
}

static void cvt_float2_float3(void *to, const void *from, const struct attrib *attr)
{
	const float *src = from;
	float *dest = to;

	dest[0] = src[0];
	dest[1] = src[1];
	dest[2] = 0.f;
}

static void cvt_short2_short3(void *to, const void *from, const struct attrib *attr)
{
	const short *src = from;
	short *dest = to;

	dest[0] = src[0];
	dest[1] = src[1];
	dest[2] = 0;
}

static void cvt_byte2_byte3(void *to, const void *from, const struct attrib *attr)
{
	const unsigned char *src = from;
	unsigned char *dest = to;

	dest[0] = src[0];
	dest[1] = src[1];
	dest[2] = 0;
}


unsigned __pspgl_enabled_array_bits(void)
{
	struct varray *v = &pspgl_curctx->vertex_array;
	unsigned ret = 0;

	if (v->vertex.enabled)
		ret |= VA_VERTEX_BIT;
	if (v->normal.enabled)
		ret |= VA_NORMAL_BIT;
	if (v->color.enabled)
		ret |= VA_COLOR_BIT;
	if (v->texcoord.enabled)
		ret |= VA_TEXCOORD_BIT;

	return ret;
}

/* 
   Examine the currently enabled vertex attribute arrays to compute a
   hardware vertex format, along with enough information to convert
   the arrays into that format.
 */
void __pspgl_ge_vertex_fmt(struct pspgl_context *ctx, struct vertex_format *vfmt)
{
	unsigned hwformat = 0;
	struct varray *varray = &ctx->vertex_array;
	struct attrib *attr = vfmt->attribs;
	unsigned offset;

	memset(vfmt, 0, sizeof(*vfmt));

	offset = 0;

	if (!varray->vertex.enabled)
		return;

	if (varray->texcoord.enabled) {
		unsigned hwtype = glfmt2gefmt(varray->texcoord.type);

		hwformat |= GE_TEXTURE_SHIFT(hwtype);

		attr->array = &varray->texcoord;
		offset = ROUNDUP(offset, ge_sizeof(hwtype));
		attr->offset = offset;
		attr->size = ge_sizeof(hwtype) * 2;
		offset += attr->size;

		attr++;
	}

	if (varray->color.enabled) {
		unsigned type = varray->color.type;
		unsigned size = varray->color.size;

		/* Always use RGBA8888 for now, but we could use other
		   color formats by extending glColorPointer to accept
		   more types (GL_UNSIGNED_SHORT_5_6_5, etc). */
		hwformat |= GE_COLOR_8888;

		assert(type == GL_FLOAT || type == GL_UNSIGNED_BYTE);
		assert(size == 3 || size == 4);

		if (type == GL_FLOAT) {
			if (size == 3)
				attr->convert = cvt_color_float3_ub;
			else
				attr->convert = cvt_color_float4_ub;
		} else if (type == GL_UNSIGNED_BYTE && size == 3)
			attr->convert = cvt_color_ub3_ub;

		attr->array = &varray->color;
		offset = ROUNDUP(offset, 4);
		attr->offset = offset;
		attr->size = 4;
		offset += 4;

		attr++;
	}

	if (varray->normal.enabled) {
		unsigned hwtype = glfmt2gefmt(varray->normal.type);

		hwformat |= GE_NORMAL_SHIFT(hwtype);

		attr->array = &varray->normal;
		offset = ROUNDUP(offset, ge_sizeof(hwtype));
		attr->offset = offset;
		attr->size = ge_sizeof(hwtype) * 3;
		offset += attr->size;

		attr++;
	}

	if (varray->vertex.enabled) {
		unsigned hwtype = glfmt2gefmt(varray->vertex.type);
		unsigned size = varray->vertex.size;

		hwformat |= GE_VERTEX_SHIFT(hwtype);

		/* size must be either 2 or 3  */
		if (size == 2) {
			switch(hwtype) {
			case GE_INT_8BIT:	attr->convert = cvt_byte2_byte3; break;
			case GE_INT_16BIT:	attr->convert = cvt_short2_short3; break;
			case GE_FLOAT_32BIT:	attr->convert = cvt_float2_float3; break;
			}
		}

		attr->array = &varray->vertex;
		offset = ROUNDUP(offset, ge_sizeof(hwtype));
		attr->offset = offset;
		attr->size = ge_sizeof(hwtype) * 3;
		offset += attr->size;

		attr++;
	}

	offset = ROUNDUP(offset, 4);

	vfmt->nattrib = attr - vfmt->attribs;
	vfmt->hwformat = hwformat;
	vfmt->vertex_size = offset;

	vfmt->arrays = __pspgl_enabled_array_bits();

	psp_log("format: %x %d attr, %d byte vertex\n",
		vfmt->hwformat, vfmt->nattrib, vfmt->vertex_size);
}

/* 
   Generate a vertex array in hardware format based on the current set
   of array pointers set up in the context.  It will copy as many
   vertices into memory at "to" as will fit into "space" (up to
   "count"), and return the number actually copied.
*/
int __pspgl_gen_varray(const struct vertex_format *vfmt, int first, int count, 
		       void *to, int space)
{
	int i;
	unsigned char *dest = to;
	int nvtx = space / vfmt->vertex_size;

	if (nvtx > count)
		nvtx = count;

	for(i = 0; i < vfmt->nattrib; i++) {
		struct pspgl_vertex_array *a = vfmt->attribs[i].array;

		if (a->enabled)
			a->tmpptr = a->ptr + (first * a->stride);
		else
			a->tmpptr = NULL;
	}

	for(i = 0; i < nvtx; i++) {
		int j;

		for(j = 0; j < vfmt->nattrib; j++) {
			const struct attrib *attr = &vfmt->attribs[j];
			struct pspgl_vertex_array *a = attr->array;

			if (attr->convert)
				(*attr->convert)(&dest[attr->offset], a->tmpptr, attr);
			else
				memcpy(&dest[attr->offset], a->tmpptr, attr->size);

			a->tmpptr += a->stride;
		}
		dest += vfmt->vertex_size;
	}

	return nvtx;
}

long __pspgl_glprim2geprim (GLenum glprim)
{
	static const char geprim_tab [] = {
		GE_POINTS, GE_LINES, GE_LINE_STRIP, GE_LINE_STRIP,
		GE_TRIANGLES, GE_TRIANGLE_STRIP, GE_TRIANGLE_FAN, 
		GE_TRIANGLE_FAN, GE_TRIANGLE_STRIP, GE_TRIANGLE_FAN
	};

	if ((unsigned) glprim > sizeof(geprim_tab)/sizeof(geprim_tab[0]))
		return -1;

	return geprim_tab[glprim];
}


/* 
   This table indicates how each primitive type uses vertices:

    - The first column indicates the amount of overlap required in
      order to split a primitive into pieces (for example, a split strip
      needs the last two vertices from the previous piece to be copied).
    - The second column indicates the minimum number of vertices needed
      to draw anything at all.
    - The third column indicates what multiple of vertices are required in
      total (every triangle needs three complete vertices, but a strip can
      make progress with single vertices after the first 2).
 */

const struct prim_info __pspgl_prim_info[] = {
	[GE_POINTS]         = { 0, 1, 1 },
	[GE_LINES]          = { 0, 2, 2 },
	[GE_SPRITES]        = { 0, 2, 2 },
	[GE_TRIANGLES]      = { 0, 3, 3 },
	[GE_LINE_STRIP]     = { 1, 2, 1 },
	[GE_TRIANGLE_STRIP] = { 2, 3, 1 },
	[GE_TRIANGLE_FAN]   = { 2, 3, 1 },
};
