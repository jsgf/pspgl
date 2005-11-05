#include "pspgl_internal.h"
#include "pspgl_texobj.h"

void glGenTextures (GLsizei n, GLuint *textures)
{
	struct hashtable *hash = &pspgl_curctx->shared->texture_objects;
	GLsizei i;

	if (n < 0) {
		GLERROR(GL_INVALID_VALUE);
		return;
	}

	for(i = 0; i < n; i++) {
		unsigned id;
		struct pspgl_texobj *tobj;

		id = __pspgl_hash_uniquekey(hash);

		tobj = __pspgl_texobj_new(id, 0);
		if (tobj == NULL) {
			GLERROR(GL_OUT_OF_MEMORY);
			return;
		}
		__pspgl_hash_insert(hash, id, tobj);

		textures[i] = id;
	}
}

