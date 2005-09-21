#include "pspgl_internal.h"


void glPrioritizeTextures (GLsizei n, const GLuint *textures, const GLclampf *priorities)
{
	struct hashtable *hash = &pspgl_curctx->shared->texture_objects;
	struct pspgl_texobj *texobj;
	GLsizei i;

	if (n < 0) {
		GLERROR(GL_INVALID_VALUE);
		return;
	}

	for (i=0; i<n; i++) {
		if ((texobj = pspgl_hash_lookup(hash, textures[i])))
			texobj->priority = priorities[i];
	}
}

