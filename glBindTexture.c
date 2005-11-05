#include <string.h>
#include "pspgl_internal.h"


void glBindTexture (GLenum target, GLuint texture)
{
	struct pspgl_context *c = pspgl_curctx;
	struct hashtable *hash = &pspgl_curctx->shared->texture_objects;
	struct pspgl_texobj *texobj = (texture == 0) ? &c->texobj0 : __pspgl_hash_lookup(hash, texture);

	if (!texobj) {
		if (!(texobj = __pspgl_texobj_new())) {
			GLERROR(GL_OUT_OF_MEMORY);
			return;
		}
		__pspgl_hash_insert(hash, texture, texobj);
		texobj->target = target;
	} else if (texobj->target != target) {
		GLERROR(GL_INVALID_OPERATION);
		return;
	}

	if (c->texobj_current != texobj) {
		uint32_t *texreg;
		unsigned long i;

		memcpy(c->texobj_current->ge_texreg_160x201, &c->ge_reg[CMD_TEX_MIPMAP0], sizeof(c->texobj_current->ge_texreg_160x201));
		c->texobj_current = texobj;

		for (i=CMD_TEX_MIPMAP0, texreg=c->texobj_current->ge_texreg_160x201; i<CMD_TEXENV_FUNC; i++, texreg++)
			__pspgl_context_writereg(c, i, *texreg);

		__pspgl_context_writereg_masked(c, CMD_TEXENV_FUNC, c->texobj0.ge_texreg_160x201[CMD_TEXENV_FUNC-CMD_TEX_MIPMAP0], 0xffff00);
	}
}

