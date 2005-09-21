#ifndef __pspgl_texobj_h__
#define __pspgl_texobj_h__

#include <GL/gl.h>
#include <GLES/egl.h>

typedef unsigned long uint32_t;

struct pspgl_texobj {
	uint32_t ge_texreg_160x201 [201-160+1];
	GLclampf  priority;
	GLenum target;
};


extern const struct pspgl_texobj pspgl_texobj_default;

extern struct pspgl_texobj* pspgl_texobj_new (void);
extern void pspgl_texobj_free (struct pspgl_texobj *t);


#endif

