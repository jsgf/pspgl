#ifndef __pspgl_internal_h__
#define __pspgl_internal_h__

#include <GL/gl.h>
#include <GLES/egl.h>

#include "guconsts.h"

#include "pspgl_dlist.h"
#include "pspgl_hash.h"
#include "pspgl_misc.h"


#define NUM_CMDLISTS	16

struct pspgl_vertex_array {
	GLenum enabled;
	GLint size;
	GLenum type;
	GLsizei stride;
	const GLvoid *ptr;
};


struct pspgl_shared_context {
	int refcount;
	struct hashtable texture_objects;
	struct hashtable display_lists;
};

struct pspgl_matrix {
	GLfloat mat[16];
};

struct pspgl_matrix_stack {
	struct pspgl_matrix *stack;
	unsigned limit;
	unsigned depth;
	unsigned dirty;		/* hardware needs updating */
};

struct pspgl_context {
	uint32_t ge_reg [256];
	uint32_t ge_reg_touched [256/32];

	struct {
		GLenum primitive;
		unsigned long vertex_count;
		void *vbuf_adr;
		GLfloat texcoord [4];
		unsigned long color;
		GLfloat normal [3];
	} current;

	struct {
		struct pspgl_vertex_array vertex;
		struct pspgl_vertex_array normal;
		struct pspgl_vertex_array color;
		struct pspgl_vertex_array texcoord;
	} vertex_array;

	struct {
		GLclampf color [4];
		GLclampf depth;
		GLint stencil;
	} clear;

	struct {
		GLenum enabled;
		GLint x, y, width, height;
	} scissor_test;

	struct {
		unsigned char alpha;
		unsigned char stencil;
	} write_mask;

	GLfloat depth_offset;


	struct pspgl_matrix_stack projection_stack;
	struct pspgl_matrix_stack modelview_stack;
	struct pspgl_matrix_stack texture_stack;

	struct pspgl_matrix_stack *current_matrix_stack;
	struct pspgl_matrix *current_matrix;


	struct pspgl_shared_context *shared;

	struct pspgl_surface *read;
	struct pspgl_surface *draw;
	
	struct pspgl_dlist *dlist[NUM_CMDLISTS];
	struct pspgl_dlist *dlist_current;
	int dlist_idx;

	GLenum glerror;
	unsigned int swap_interval;
	int initialized;
	int refcount;

	/* XXX IMPROVE Do we really need to store anything below? these are hardware states, stored in ge_reg[]... */
	struct {
		GLint x, y, width, height;
	} viewport;
	struct {
		GLfloat near, far;
	} fog;
 	struct {
		struct pspgl_texobj	*bound;	/* currently bound texture */
 	} texture;
};


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
extern void __pspgl_ge_init (struct pspgl_context *c);


/* pspgl_vidmem.c */
extern EGLint __pspgl_eglerror;
extern struct pspgl_context *__pspgl_curctx;
#define pspgl_curctx	__pspgl_curctx

extern void* __pspgl_vidmem_alloc (unsigned long size);
extern void  __pspgl_vidmem_free (void * ptr);
extern EGLBoolean __pspgl_vidmem_setup_write_and_display_buffer (struct pspgl_surface *s);


/* pspgl_varray.c */
extern long __pspgl_glprim2geprim (GLenum glprim);
extern void __pspgl_varray_draw (GLenum mode, GLenum index_type, const GLvoid *indices, GLint first, GLsizei count);


/* glTexImage2D.c */
struct pspgl_teximg;
extern void __pspgl_set_texture_image(struct pspgl_texobj *tobj, unsigned level, struct pspgl_teximg *timg);

static inline unsigned ispow2(unsigned n)
{
	return (n & (n-1)) == 0;
}


#define GLERROR(errcode)					\
do {								\
	__pspgl_log("*** GL error 0x%04x ***\n", errcode);		\
	if (__pspgl_curctx)						\
		__pspgl_curctx->glerror = errcode;			\
} while (0)


#define EGLERROR(errcode)					\
do {								\
	__pspgl_log("*** EGL error 0x%04x ***\n",	errcode);	\
	__pspgl_eglerror = errcode;					\
} while (0)


static inline GLclampf CLAMPF (GLfloat x)
{
	return (x < 0.0 ? 0.0 : x > 1.0 ? 1.0 : x);
}


static inline
unsigned long COLOR3 (const GLfloat c[3])
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

extern const GLfloat __pspgl_identity[];

extern void __pspgl_context_writereg (struct pspgl_context *c, unsigned long cmd, unsigned long argi);
extern void __pspgl_context_writereg_masked (struct pspgl_context *c, unsigned long cmd, unsigned long argi, unsigned long mask);
extern void __pspgl_context_flush_pending_state_changes (struct pspgl_context *c);
extern void __pspgl_context_writereg_uncached (struct pspgl_context *c, unsigned long cmd, unsigned long argi);

extern void __pspgl_context_flush_pending_matrix_changes (struct pspgl_context *c);

#define sendCommandi(cmd,argi)		__pspgl_context_writereg(pspgl_curctx, cmd, argi)
#define sendCommandiUncached(cmd,argi)	__pspgl_context_writereg_uncached (pspgl_curctx, cmd, argi)

#define sendCommandf(cmd,argf)						\
do {									\
	union { float f; int i; } arg = { .f = argf };			\
	sendCommandi(cmd, arg.i >> 8);					\
} while (0)

static inline uint32_t getReg(reg)
{
	return pspgl_curctx->ge_reg[reg];
}


#endif

