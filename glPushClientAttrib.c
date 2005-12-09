#include <stdlib.h>
#include <string.h>

#include "pspgl_internal.h"
#include "pspgl_buffers.h"

struct pspgl_saved_clattrib {
	GLbitfield attrmask;

	struct varray varray;
	struct texture texture;
};

void glPushClientAttrib(GLbitfield mask)
{
	struct pspgl_context *c = pspgl_curctx;
	struct pspgl_saved_clattrib *a;

	if (c->attribstackdepth >= MAX_ATTRIB_STACK) {
		GLERROR(GL_STACK_OVERFLOW);
		return;
	}

	a = malloc(sizeof(*a));

	if (a == NULL) {
		GLERROR(GL_OUT_OF_MEMORY);
		return;
	}

	memset(a, 0, sizeof(*a));

	a->attrmask = mask;

	if (mask & GL_CLIENT_VERTEX_ARRAY_BIT) {
		a->varray = c->vertex_array;

		if (a->varray.vertex.buffer)
			a->varray.vertex.buffer->refcount++;

		if (a->varray.normal.buffer)
			a->varray.normal.buffer->refcount++;

		if (a->varray.color.buffer)
			a->varray.color.buffer->refcount++;

		if (a->varray.texcoord.buffer)
			a->varray.texcoord.buffer->refcount++;

		if (a->varray.weight.buffer)
			a->varray.weight.buffer->refcount++;

		if (a->varray.arraybuffer)
			a->varray.arraybuffer->refcount++;
		if (a->varray.indexbuffer)
			a->varray.indexbuffer->refcount++;

		if (a->varray.locked.cached_array)
			a->varray.locked.cached_array->refcount++;
	}

	if (mask & GL_CLIENT_PIXEL_STORE_BIT) {
		a->texture.unpackbuffer = c->texture.unpackbuffer;
		a->texture.packbuffer = c->texture.packbuffer;

		if (a->texture.unpackbuffer)
			a->texture.unpackbuffer->refcount++;
		if (a->texture.packbuffer)
			a->texture.packbuffer->refcount++;
	}

	c->clattribstack[c->clattribstackdepth++] = a;
}

void glPopClientAttrib(void)
{
	struct pspgl_context *c = pspgl_curctx;
	struct pspgl_saved_clattrib *a;
	unsigned mask;

	if (c->clattribstackdepth == 0) {
		GLERROR(GL_STACK_UNDERFLOW);
		return;
	}

	a = c->clattribstack[--c->clattribstackdepth];

	mask = a->attrmask;

	if (mask & GL_CLIENT_VERTEX_ARRAY_BIT) {
		struct varray *va = &c->vertex_array;

		if (va->vertex.buffer)
			__pspgl_bufferobj_free(va->vertex.buffer);

		if (va->normal.buffer)
			__pspgl_bufferobj_free(va->normal.buffer);

		if (va->color.buffer)
			__pspgl_bufferobj_free(va->color.buffer);

		if (va->texcoord.buffer)
			__pspgl_bufferobj_free(va->texcoord.buffer);

		if (va->weight.buffer)
			__pspgl_bufferobj_free(va->weight.buffer);

		if (va->arraybuffer)
			__pspgl_bufferobj_free(va->arraybuffer);
		if (va->indexbuffer)
			__pspgl_bufferobj_free(va->indexbuffer);

		if (va->locked.cached_array)
			__pspgl_buffer_free(va->locked.cached_array);

		*va = a->varray;
	}

	if (mask & GL_CLIENT_PIXEL_STORE_BIT) {
		if (c->texture.unpackbuffer)
			__pspgl_bufferobj_free(c->texture.unpackbuffer);
		c->texture.unpackbuffer = a->texture.unpackbuffer;

		if (c->texture.packbuffer)
			__pspgl_bufferobj_free(c->texture.packbuffer);
		c->texture.packbuffer = a->texture.packbuffer;
	}

	free(a);
}
