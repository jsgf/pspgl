#include "pspgl_internal.h"


void glGenTextures (GLsizei n, GLuint *textures)
{
	struct hashtable *hash = &pspgl_curctx->shared->texture_objects;
	GLsizei i;

	if (n < 0) {
		GLERROR(GL_INVALID_VALUE);
		return;
	}

	for(i=0; i<n; i++) {
		unsigned long id = __pspgl_hash_uniquekey(hash);
		if (id != HASH_NO_KEY) {
			textures[i] = id;
			__pspgl_hash_insert(hash, id, NULL);
		}
	}
}

