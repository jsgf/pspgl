#include <string.h>
#include "pspgl_internal.h"


void glBindTexture (GLenum target, GLuint texture)
{
	struct pspgl_context *c = pspgl_curctx;
	struct hashtable *hash = &pspgl_curctx->shared->texture_objects;
	struct pspgl_texobj *texobj = (texture == 0) ? &c->texobj0 : pspgl_hash_lookup(hash, texture);
	unsigned long i;

	if (!texobj) {
		if (!(texobj = pspgl_texobj_new())) {
			GLERROR(GL_OUT_OF_MEMORY);
			return;
		}
		pspgl_hash_insert(hash, texture, texobj);
		texobj->target = target;
	} else if (texobj->target != target) {
		GLERROR(GL_INVALID_OPERATION);
		return;
	}

	if (c->texobj_current != texobj) {
		memcpy(c->texobj_current->ge_texreg_160x201, &c->ge_reg[160], sizeof(c->texobj_current->ge_texreg_160x201));
		c->texobj_current = texobj;
		for (i=0; i<sizeof(c->texobj0.ge_texreg_160x201)/sizeof(c->texobj0.ge_texreg_160x201[0])-1; i++)
			pspgl_dlist_enqueue_cmd(c->dlist_current, c->texobj0.ge_texreg_160x201[i]);
	}

	pspgl_context_writereg_masked(c, 201, c->texobj0.ge_texreg_160x201[201-160], 0xffff00);
}

