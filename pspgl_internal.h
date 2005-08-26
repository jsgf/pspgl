#ifndef __pspgl_internal_h__
#define __pspgl_internal_h__

#include <GL/gl.h>
#include <GLES/egl.h>

typedef unsigned long uint32_t;

#include "pspgl_dlist.h"
#include "pspgl_misc.h"


#define NUM_CMDLISTS	4

struct pspgl_context {
	uint32_t ge_ctx [512];

	struct {
		GLenum primitive;
		unsigned long vertex_count;
		void *vbuf_adr;
		GLfloat texcoord [4];
		unsigned long color;
		GLfloat normal [3];
	} current;
	struct {
		struct {
			GLenum enabled;
			GLint size;
			GLenum type;
			GLsizei stride;
			const GLvoid *ptr;
		} vertex;
		struct {
			GLenum enabled;
			GLenum type;
			GLsizei stride;
			const GLvoid *ptr;
		} normal;
		struct {
			GLenum enabled;
			GLint size;
			GLenum type;
			GLsizei stride;
			const GLvoid *ptr;
		} color;
		struct {
			GLenum enabled;
			GLint size;
			GLenum type;
			GLsizei stride;
			const GLvoid *ptr;
		} texcoord;
	} vertex_array;
	struct {
		GLclampf color [4];
		GLclampf depth;
		GLint stencil;
	} clear;

	/* XXX IMPROVE Do we really need to store the viewport? it's a hardware state... */
	struct {
		GLint x, y, width, height;
	} viewport;
	GLfloat depth_offset;
	struct {
		GLenum enabled;
		GLint x, y, width, height;
	} scissor_test;
	GLenum matrix_mode;
	GLint matrix_depth[3];
	GLfloat matrix [3][16];
	struct {
		GLenum equation;
		GLenum sfactor, dfactor;
	} blend;
	struct {
		GLfloat near, far;
		GLclampf color [4];
	} fog;
	struct {
		GLclampf ambient [4];
		GLclampf diffuse [4];
		GLclampf specular [4];
		GLfloat position [4];
		GLfloat spot_direction [3];
		GLfloat spot_exponent;
		GLfloat spot_cutoff;
		GLfloat constant_attentuation;
		GLfloat linear_attentuation;
		GLfloat quadratic_attentuation;
	} light [4];
	struct {
		GLclampf ambient [4];
		GLenum color_control;
	} light_model;
	struct {
		GLclampf ambient [4];
		GLclampf diffuse [4];
		GLclampf specular [4];
		GLfloat shininess;
	} material;
	struct {
		GLenum enabled;
		GLenum wrap_s;
		GLenum wrap_t;
		GLenum mag_filter;
		GLenum min_filter;
	} texture;

	struct pspgl_surface *read;
	struct pspgl_surface *draw;
	
	struct pspgl_dlist *dlist[NUM_CMDLISTS];
	struct pspgl_dlist *dlist_current;
	int dlist_idx;

	GLenum glerror;
	unsigned int swap_interval;
	int initialized;
	int refcount;
};

#define NUM_CMDLISTS	4

struct pspgl_surface {
	int pixfmt;
	unsigned long width;
	unsigned long height;
	unsigned long pixelperline;
	void *color_buffer [2];
	void *depth_buffer;
	int current_front;
	int displayed;
};


/* pspgl_ge_init.c */
extern void pspgl_ge_init (struct pspgl_context *c);


/* pspgl_vidmem.c */
extern EGLint eglerror;
extern struct pspgl_context *pspgl_curctx;

extern void* pspgl_vidmem_alloc (unsigned long size);
extern void  pspgl_vidmem_free (void * ptr);
extern EGLBoolean pspgl_vidmem_setup_write_and_display_buffer (struct pspgl_surface *s);


/* pspgl_varray.c */
extern long glprim2geprim (GLenum glprim);
extern void pspgl_varray_draw (GLenum mode, GLenum index_type, const GLvoid *indices, GLint first, GLsizei count);


/* glEnable.c */
extern void pspgl_enable_state (GLenum cap, int enable);


#define GLERROR(errcode)					\
do {								\
	psp_log("*** GL error 0x%04x ***\n", errcode);		\
	if (pspgl_curctx)					\
		pspgl_curctx->glerror = errcode;		\
} while (0)


#define EGLERROR(errcode)					\
do {								\
	psp_log("*** EGL error 0x%04x ***\n",	errcode);	\
	eglerror = errcode;					\
} while (0)


static inline GLclampf CLAMPF (GLfloat x)
{
	return (x < 0.0 ? 0.0 : x > 1.0 ? 1.0 : x);
}


static inline
unsigned long COLOR (const GLfloat c[3])
{
	return ((((int) (255.0 * CLAMPF(c[2]))) << 16) |
		(((int) (255.0 * CLAMPF(c[1]))) << 8) |
		 ((int) (255.0 * CLAMPF(c[0]))));
}


static inline
unsigned long COLOR4 (const GLfloat c[4])
{
	return ((((int) (255.0 * CLAMPF(c[3]))) << 24) |
		(((int) (255.0 * CLAMPF(c[2]))) << 16) |
		(((int) (255.0 * CLAMPF(c[1]))) << 8) |
		 ((int) (255.0 * CLAMPF(c[0]))));
}

/*
#define sendCommandi(cmd,argi)						\
do {									\
	pspgl_dlist_enqueue_cmd(pspgl_curctx->dlist_current,		\
				((cmd) << 24) | ((argi) & 0xffffff));	\
} while (0)
*/

void sendCommandi(unsigned long cmd, unsigned long argi); 

#define sendCommandf(cmd,argf)						\
do {									\
	union { float f; int i; } arg = { .f = argf };			\
	sendCommandi(cmd, arg.i >> 8);					\
} while (0)


#endif

