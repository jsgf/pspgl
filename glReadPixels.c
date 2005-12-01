#include <string.h>
#include <psputils.h>

#include "pspgl_internal.h"
#include "pspgl_texobj.h"

void glReadPixels( GLint x, GLint y,
		   GLsizei width, GLsizei height,
		   GLenum format, GLenum type,
		   GLvoid *pixels )
{
	const struct pspgl_texfmt *fmt;	
	struct pspgl_surface *read;
	int dest_x, dest_y;
	GLsizei dest_stride;

	read = pspgl_curctx->read;

	fmt = __pspgl_hardware_format(__pspgl_texformats, format, type);

	if (fmt->hwformat != read->pixfmt) {
		GLERROR(GL_INVALID_ENUM);
		return;
	}

	if ((fmt->flags & TF_NATIVE) == 0) {
		GLERROR(GL_INVALID_OPERATION);
		return;
	}

	dest_x = 0;
	dest_y = 0;
	dest_stride = width;

	if (x < 0) {
		x = -x;
		dest_x += x;
		width -= x;
		x = 0;
	}
	if ((x + width) > read->width)
		width -= (x + width) - read->width;

	if (y < 0) {
		y = -y;
		dest_y += y;
		height -= y;
		y = 0;
	}
	if ((y + height) > read->height)
		height -= (y + height) - read->height;

	if (width <= 0 || height <= 0) {
		GLERROR(GL_INVALID_VALUE);
		return;
	}

	/* The framebuffer and the texture are upside down with
	   respect to each other, so we need to flip the image (in the
	   framebuffer, lower addresses are in the upper-left, but for
	   textures, lower addresses are lower-left). */
	y = read->height - y;

	int current_back = (read->color_buffer[1] == NULL) ? 0 : (read->current_front ^ 1);
	struct pspgl_buffer *framebuffer = read->color_buffer[current_back];

	if (((unsigned)pixels & 0xf) != 0) {
		/* Unaligned dest buffer; we can't use DMA */
		const void *map;
		const void *src;
		void *dst;
		unsigned src_stride;

		src_stride = read->pixelperline * fmt->hwsize;
		dest_stride *= fmt->hwsize;

		__pspgl_buffer_dlist_sync(framebuffer);

		map = __pspgl_buffer_map(framebuffer, GL_READ_ONLY_ARB);
		src = map + (y * read->pixelperline + x) * fmt->hwsize;
		dst = pixels + (dest_y * dest_stride + dest_y) * fmt->hwsize;

		while(height--) {
			memcpy(dst, src, width * fmt->hwsize);
			dst += dest_stride;
			src -= src_stride;
		}

		__pspgl_buffer_unmap(framebuffer, GL_READ_ONLY_ARB);
	} else {
		/* Make sure the cache has no aliased content before the DMA transfer.
		   XXX Use a pspgl_buffer for this, with a pointer to an transient? */
		sceKernelDcacheWritebackInvalidateRange(pixels, width*height*fmt->hwsize);

		__pspgl_copy_pixels(framebuffer->base, -read->pixelperline, x, y,
				    pixels, dest_stride, 0, 0,
				    width, height, read->pixfmt);

		/* Sync with copy.  XXX check for use of a PBO to make this unnecessary. */
		glFinish();
	}
}
