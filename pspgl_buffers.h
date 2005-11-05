#ifndef _PSPGL_BUFFERS_H
#define _PSPGL_BUFFERS_H

#include "pspgl_internal.h"

struct pspgl_buffer {
	int refcount;

	void *base;
	GLsizeiptr size;

	void (*free)(struct pspgl_buffer *);
};

struct pspgl_bufferobj {
	int refcount;

	GLenum target;
	GLenum usage, access;
	GLboolean mapped;

	struct pspgl_buffer *data;
};

/* Create new buffer, but does not allocate any storage for it.
   Returns a buffer with a refcount of 1 */
struct pspgl_bufferobj *__pspgl_bufferobj_new(GLenum target, struct pspgl_buffer *data);

/* Decrements refcount, and frees if it hits 0 */
void __pspgl_bufferobj_free(struct pspgl_bufferobj *);

void *__pspgl_bufferobj_deref(const struct pspgl_bufferobj *buf, void *offset);

struct pspgl_bufferobj **__pspgl_bufferobj_for_target(GLenum target);

struct pspgl_buffer *__pspgl_buffer_new(void *base, GLsizeiptr size,
						void (*free)(struct pspgl_buffer *));
void __pspgl_buffer_free(struct pspgl_buffer *data);

void *__pspgl_buffer_map(struct pspgl_buffer *data, GLenum access);
void  __pspgl_buffer_unmap(struct pspgl_buffer *data, GLenum access);

#endif	/* PSPGL_BUFFERS_H */
