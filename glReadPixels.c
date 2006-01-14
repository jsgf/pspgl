#include <string.h>
#include <psputils.h>
#include <pspge.h>

#include "pspgl_internal.h"
#include "pspgl_texobj.h"

void glReadPixels( GLint x, GLint y,
		   GLsizei width, GLsizei height,
		   GLenum format, GLenum type,
		   GLvoid *pixels )
{
	struct pspgl_surface *read;
	struct pspgl_buffer *framebuffer;
	unsigned fb_offset;
	unsigned hwsize;
	unsigned pixfmt;
	int dest_x, dest_y;
	GLsizei dest_stride;

	read = pspgl_curctx->read;

	if (format != GL_DEPTH_COMPONENT) {
		const struct pspgl_texfmt *fmt;	

		fmt = __pspgl_hardware_format(__pspgl_texformats, format, type);

		if (fmt->hwformat != read->pixfmt) {
			GLERROR(GL_INVALID_ENUM);
			return;
		}

		if ((fmt->flags & TF_NATIVE) == 0) {
			GLERROR(GL_INVALID_OPERATION);
			return;
		}

		framebuffer = read->color_back;
		fb_offset = 0;

		hwsize = fmt->hwsize;
		pixfmt = read->pixfmt;
	} else {
		if (read->depth_buffer == NULL) {
			GLERROR(GL_INVALID_OPERATION);
			return;
		}
		if (type != GL_UNSIGNED_SHORT) {
			GLERROR(GL_INVALID_ENUM);
			return;
		}

		/* The PSP provides a window into VRAM which makes the
		   depth buffer look properly linearized. */
		framebuffer = read->depth_buffer;
		fb_offset = sceGeEdramGetSize() * 3;

		hwsize = 2;
		pixfmt = GE_RGBA_4444; /* any 16-bit sized format */
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

	if (((unsigned)pixels & 0xf) != 0) {
		/* Unaligned dest buffer; we can't use DMA */
		const void *map;
		const void *src;
		void *dst;
		unsigned src_stride;

		src_stride = read->pixelperline * hwsize;
		dest_stride *= hwsize;

		__pspgl_buffer_dlist_sync(framebuffer);

		map = __pspgl_buffer_map(framebuffer, GL_READ_ONLY_ARB);
		src = map + fb_offset + (y * read->pixelperline + x) * hwsize;
		dst = pixels + (dest_y * dest_stride + dest_y) * hwsize;

		while(height--) {
			memcpy(dst, src, width * hwsize);
			dst += dest_stride;
			src -= src_stride;
		}

		__pspgl_buffer_unmap(framebuffer, GL_READ_ONLY_ARB);
	} else {
		/* Make sure the cache has no aliased content before the DMA transfer.
		   XXX Use a pspgl_buffer for this, with a pointer to an transient? */
		sceKernelDcacheWritebackInvalidateRange(pixels, width*height*hwsize);

		__pspgl_copy_pixels(framebuffer->base + fb_offset, -read->pixelperline, x, y,
				    pixels, dest_stride, 0, 0,
				    width, height, pixfmt);

		/* Sync with copy.  XXX check for use of a PBO to make this unnecessary. */
		glFinish();
	}
}
