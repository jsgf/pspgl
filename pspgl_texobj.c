#include <stdlib.h>
#include <string.h>
#include "pspgl_misc.h"
#include "pspgl_texobj.h"

static void tobj_setstate(struct pspgl_texobj *tobj, unsigned reg, unsigned setting)
{
	assert(reg >= TEXSTATE_START && reg <= TEXSTATE_END);

	tobj->ge_texreg[reg - TEXSTATE_START] = (reg << 24) | (setting & 0xffffff);
}


struct pspgl_texobj *__pspgl_texobj_new(GLint id, GLenum target)
{
	struct pspgl_texobj *tobj = malloc(sizeof(*tobj));
	unsigned i;

	memset(tobj, 0, sizeof(*tobj));

	tobj->refcount = 1;
	tobj->target = target;

	for(i = TEXSTATE_START; i <= TEXSTATE_END; i++)
		tobj_setstate(tobj, i, 0);

	tobj_setstate(tobj, CMD_TEXFILT, (GE_TEX_FILTER_LINEAR << 8) | GE_TEX_FILTER_LINEAR);
	tobj_setstate(tobj, CMD_TEXWRAP, (GE_TEX_WRAP_REPEAT << 8) | GE_TEX_WRAP_REPEAT);

	return tobj;
}

void __pspgl_texobj_free(struct pspgl_texobj *tobj)
{
	int i;

	if (--tobj->refcount)
		return;

	for(i = 0; i < MIPMAP_LEVELS; i++)
		if (tobj->images[i] != NULL)
			__pspgl_teximg_free(tobj->images[i]);

	free(tobj);
}


struct pspgl_teximg *__pspgl_teximg_new(const void *pixels, int width, int height,
					const struct pspgl_texfmt *texfmt)
{
	struct pspgl_teximg *timg;
	unsigned size;

	timg = malloc(sizeof(*timg));

	memset(timg, 0, sizeof(*timg));
	timg->refcount = 1;

	size = width * height * texfmt->hwsize;

	timg->image = malloc(size); /* XXX try to allocate edram */

	__pspgl_convert_image(pixels, width, height, timg->image, texfmt);

	/* Make texture data visible to hardware */
	sceKernelDcacheWritebackRange(timg->image, size);

	timg->width = width;
	timg->height = height;
	timg->stride = width;
	timg->texfmt = texfmt;

	return timg;
}

void __pspgl_teximg_free(struct pspgl_teximg *timg)
{
	if (--timg->refcount)
		return;

	free(timg->image);
	free(timg);
}
