#include <stdlib.h>
#include "pspgl_internal.h"
#include "pspgl_texobj.h"

void glDeleteTextures (GLsizei n, const GLuint *textures)
{
	struct hashtable *hash = &pspgl_curctx->shared->texture_objects;
	GLsizei i;

	if (n < 0) {
		GLERROR(GL_INVALID_VALUE);
		return;
	}

	for (i=0; i<n; i++) {
		if (textures[i] != 0) {
			struct pspgl_texobj *tobj = __pspgl_hash_remove(hash, textures[i]);
			if (tobj) {
				if (tobj == pspgl_curctx->texture.bound)
					glBindTexture(tobj->target, 0);
				free(tobj);
			}
		}
	}
}

