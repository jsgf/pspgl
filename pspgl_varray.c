#include "pspgl_internal.h"


#define GE_COLOR_RES1  1
#define GE_COLOR_RES2  2
#define GE_COLOR_RES3  3
#define GE_COLOR_5650  4
#define GE_COLOR_5551  5
#define GE_COLOR_4444  6
#define GE_COLOR_8888  7

#define GE_INT_8BIT    1
#define GE_INT_16BIT   2
#define GE_FLOAT_32BIT 3


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


static
unsigned long ge_vertex_fmt (struct pspgl_context *ctx, unsigned long *adr)
{
	unsigned long stride;
	unsigned long next;
	unsigned long ge_type;
	unsigned long fmt = 0;

	next = *adr = 0;

	if (ctx->vertex_array.texcoord.enabled) {
		if (ctx->vertex_array.texcoord.size != 2)
			goto bailout;

		if ((ge_type = glfmt2gefmt(ctx->vertex_array.texcoord.type)) <= 0)
			goto bailout;

		next = *adr = (unsigned long) ctx->vertex_array.texcoord.ptr;
		next += 2 * (1 << (ge_type - 1));
		fmt |= ge_type;
	}

	if (ctx->vertex_array.color.enabled) {
		if (ctx->vertex_array.color.size != 4)
			goto bailout;

		if (next && next != (unsigned long) ctx->vertex_array.color.ptr)
			goto bailout;

		if (ctx->vertex_array.color.size != 4)
			goto bailout;

		switch (ctx->vertex_array.color.type) {
		case GL_UNSIGNED_BYTE:
			if (*adr == 0)
				next = *adr = (unsigned long) ctx->vertex_array.color.ptr;
			next += 4;
			fmt |= GE_COLOR_8888 << 2;
			break;
		default:
			goto bailout;
		}
	}

	if (ctx->vertex_array.normal.enabled) {
		if (next && next != (unsigned long) ctx->vertex_array.normal.ptr)
			goto bailout;

		if ((ge_type = glfmt2gefmt(ctx->vertex_array.normal.type)) <= 0)
			goto bailout;

		if (*adr == 0)
			*adr = (unsigned long) ctx->vertex_array.normal.ptr;
		next += 3 * (1 << (ge_type - 1));
		fmt |= (ge_type << 5);
	}

	if (ctx->vertex_array.vertex.enabled) {
		if (next && next != (unsigned long) ctx->vertex_array.vertex.ptr)
			goto bailout;

		if (ctx->vertex_array.vertex.size != 3)
			goto bailout;

		if ((ge_type = glfmt2gefmt(ctx->vertex_array.vertex.type)) <= 0)
			goto bailout;

		if (*adr == 0)
			*adr = (unsigned long) ctx->vertex_array.vertex.ptr;
		next += 3 * (1 << (ge_type - 1));
		fmt |= (ge_type << 7);
	}

	if (*adr & 0x0f)		/* not 16-byte aligned! */
		goto bailout;

	/* check that all array strides are equal and word-aligned */
	stride = ((next - *adr) + 0x03) & ~0x03;

	if (ctx->vertex_array.vertex.enabled && ctx->vertex_array.vertex.stride != stride)
		goto bailout;

	if (ctx->vertex_array.normal.enabled && ctx->vertex_array.normal.stride != stride)
		goto bailout;

	if (ctx->vertex_array.color.enabled && ctx->vertex_array.color.stride != stride)
		goto bailout;

	if (ctx->vertex_array.texcoord.enabled && ctx->vertex_array.texcoord.stride != stride)
		goto bailout;

	return fmt;

bailout:
	return 0;
}


#define GE_POINTS		0
#define GE_LINES		1
#define GE_LINE_STRIP		2
#define GE_TRIANGLES		3
#define GE_TRIANGLE_STRIP	4
#define GE_TRIANGLE_FAN		5
#define GE_QUADS		6


long glprim2geprim (GLenum glprim)
{
	static const char geprim_tab [] = {
		GE_POINTS, GE_LINES, GE_LINE_STRIP, GE_LINE_STRIP,
		GE_TRIANGLES, GE_TRIANGLE_STRIP, GE_TRIANGLE_FAN, 
		GE_QUADS, GE_TRIANGLE_STRIP, GE_TRIANGLE_FAN
	};

	if ((unsigned) glprim > sizeof(geprim_tab)/sizeof(geprim_tab[0]))
		return -1;

	return geprim_tab[glprim];
}


void pspgl_varray_draw (GLenum mode, GLenum index_type, const GLvoid *indices, GLint first, GLsizei count)
{
	unsigned long adr;
	unsigned long vertex_fmt = ge_vertex_fmt(pspgl_curctx, &adr);
	long prim = glprim2geprim(mode);

	if (prim < 0 || vertex_fmt == 0) {
		GLERROR(GL_INVALID_ENUM);
		return;
	}

	if (indices) {
		switch (index_type) {
		case GL_UNSIGNED_BYTE:
			vertex_fmt |= (GE_INT_8BIT << 11);
			break;
		case GL_UNSIGNED_SHORT:
			vertex_fmt |= (GE_INT_16BIT << 11);
			break;
		/*  XXX : can we handle 32 bit integer indices??
		case GL_UNSIGNED_INT:
			vertex_fmt |= (GE_INT_32BIT << 11);
			break;
		*/
		default:
			GLERROR(GL_INVALID_ENUM);
			return;
		}
		sendCommandi(16, (((unsigned long) indices) >> 8) & 0xf0000);
		sendCommandi(2, ((unsigned long) indices) & 0xffffff);
	}

	sendCommandi(18, vertex_fmt);
	sendCommandi(16, (adr >> 8) & 0xf0000);
	sendCommandi(1, adr & 0xffffff);
	sendCommandi(4, (prim << 16) | count);

	/* XXX TODO: we handle line loops as line strips. Here we need to render the final, closing line, too. */
}

