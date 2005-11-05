#include <GL/glu.h>
#include <GL/gl.h>
#include <stdlib.h>
#include <string.h>

#include "pspglu.h"

static unsigned pow2(unsigned x)
{
	unsigned ret = 1;

	while(x > ret)
		ret <<= 1;

	return ret;
}

static int lg2(int x)
{
	int ret = -1;

	if (x == 0 || ((x-1) & x))
		return -1;

	while(x) {
		x >>= 1;
		ret++;
	}

	return ret;
}

GLint gluBuild2DMipmaps( GLenum target,
			 GLint internalFormat,
			 GLsizei width,
			 GLsizei height,
			 GLenum format,
			 GLenum type,
			 const void *data )
{
	const struct format *fmt = __pspglu_getformat(format, type);
	GLsizei tw, th;
	const void *src;
	void *dst;
	int level, levels;
	GLint maxtex;

	if (fmt == NULL)
		return GLU_INVALID_ENUM;

	tw = pow2(width);
	th = pow2(height);

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxtex);

	while(tw > maxtex)
		tw /= 2;
	while(th > maxtex)
		th /= 2;	

	levels = lg2(tw);
	level = lg2(th);
	if (level > levels)
		levels = level;

	src = data;
	dst = NULL;

	for(level = 0; level <= levels; level++) {
		dst = malloc(tw * th * fmt->size);

		gluScaleImage(format,
			      width, height, type, src,
			      tw, th, type, dst);

		glTexImage2D(target, level, internalFormat, 
			     tw, th, 0, format, type, dst);

		if (src != data)
			free((void *)src);
		src = dst;
		width = tw;
		height = th;

		if (tw > 1)
			tw /= 2;
		if (th > 1)
			th /= 2;
	}
	free(dst);

	return 0;
}
