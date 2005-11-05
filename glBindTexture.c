#include <string.h>
#include "pspgl_internal.h"
#include "pspgl_texobj.h"

static void dlist_cleanup_teximg(void *v)
{
	struct pspgl_teximg *timg = v;

	psp_log("dlist cleanup of timg %p(%d)->image=%p\n", timg, timg->refcount, timg->image);
	__pspgl_teximg_free(timg);
}

void glBindTexture(GLenum target, GLuint id)
{
	struct pspgl_texobj *tobj, *bound;
	struct hashtable *hash = &pspgl_curctx->shared->texture_objects;
	unsigned i;

	bound = pspgl_curctx->texture.bound;
	tobj = __pspgl_hash_lookup(hash, id);

	if (tobj != NULL) {
		if (tobj->target == 0)
			tobj->target = target;
		else if (tobj->target != target) {
			GLERROR(GL_INVALID_OPERATION);
			return;
		}
	} else {
		/* if this is a new id, create the texture */
		tobj = __pspgl_texobj_new(id, target);
		psp_log("id %u unknown; creating new texture %p\n", id, tobj);
		if (tobj == NULL) {
			GLERROR(GL_OUT_OF_MEMORY);
			return;
		}
		__pspgl_hash_insert(hash, id, tobj);
	}

	if (bound == tobj)
		return;

	if (bound != NULL) {
		int i;

		psp_log("unbinding previous texture %p\n", bound);

		/* save per-texture state in texture object */
		for(i = TEXSTATE_START; i <= TEXSTATE_END; i++)
			bound->ge_texreg[i - TEXSTATE_START] = getReg(i);

		/* We're about to remove the hardware's references to
		   this texture's images and color map, so make sure
		   the references are cleaned up when the hardware's
		   actually finished with them. */
		for(i = 0; i < MIPMAP_LEVELS; i++)
			if (bound->images[i])
				__pspgl_dlist_set_cleanup(dlist_cleanup_teximg, bound->images[i]);
		if (bound->cmap)
			__pspgl_dlist_set_cleanup(dlist_cleanup_teximg, bound->cmap);

		__pspgl_texobj_free(bound);
	}

	psp_log("binding to %u (%p)\n", id, tobj);

	tobj->refcount++;
	pspgl_curctx->texture.bound = tobj;

	/* restore texture state */
	psp_log("updating register state\n");
	for(i = TEXSTATE_START; i <= TEXSTATE_END; i++)
		sendCommandi(i, tobj->ge_texreg[i - TEXSTATE_START]);

	/* Update the refcounts now that hardware is referring to the
	   new images. */
	for(i = 0; i < MIPMAP_LEVELS; i++)
		if (tobj->images[i])
			tobj->images[i]->refcount++;
	if (tobj->cmap)
		tobj->cmap->refcount++;

	__pspgl_update_texenv(tobj);
	sendCommandi(CMD_TEXCACHE_FLUSH, getReg(CMD_TEXCACHE_FLUSH)+1);
}
