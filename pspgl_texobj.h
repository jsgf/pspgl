#ifndef __pspgl_texobj_h__
#define __pspgl_texobj_h__

#include <GL/gl.h>
#include <GLES/egl.h>

#include <pspgl_buffers.h>
#include "guconsts.h"

#define MIPMAP_LEVELS		8

struct pspgl_texfmt {
	GLenum format;
	GLenum type;
	unsigned pixsize;

	unsigned hwformat;
	unsigned hwsize;

	void (*convert)(const struct pspgl_texfmt *, void *to, const void *from, unsigned width);

	unsigned flags;
#define TF_ALPHA	(1<<0)
#define TF_NATIVE	(1<<1)

	struct pspgl_teximg *cmap;
};


struct pspgl_texobj {
	unsigned	refcount;
	GLenum		target;		/* either 0 or GL_TEXTURE_2D */
	GLclampf	priority;

#define TEXOBJ_NTEXREG	(TEXSTATE_END - TEXSTATE_START + 1)
	uint32_t	ge_texreg[TEXOBJ_NTEXREG];
	const struct pspgl_texfmt *texfmt;

	struct pspgl_teximg *cmap;

	struct pspgl_teximg *images[MIPMAP_LEVELS];
};

struct pspgl_teximg {
	struct pspgl_buffer	*image;		/* image pixels */
	unsigned		offset;		/* byte offset of pixels in image buffer */

	struct pspgl_buffer	*srcbuffer;	/* if the src format is native, then this ==image */
	unsigned		srcoffset;
	short			 srcgeneration;	/* generation when the src image was converted */

	unsigned	width, height;

	const struct pspgl_texfmt *texfmt;
};

extern const struct pspgl_texfmt __pspgl_texformats[];

extern struct pspgl_texobj* __pspgl_texobj_new (GLuint id, GLenum target);
extern void __pspgl_texobj_free (struct pspgl_texobj *t);

extern struct pspgl_teximg *__pspgl_teximg_new(const void *pixels, struct pspgl_bufferobj *buffer,
					       unsigned width, unsigned height, unsigned size,
					       const struct pspgl_texfmt *texfmt);
extern void __pspgl_teximg_free(struct pspgl_teximg *timg);

extern void __pspgl_update_texenv(struct pspgl_texobj *tobj);

extern const struct pspgl_texfmt *__pspgl_hardware_format(const struct pspgl_texfmt *, GLenum format, GLenum type);

#endif

