#include "pspgl_internal.h"

static inline int absi(int x)
{
	return (x < 0) ? -x : x;
}

static inline int sgni(int x)
{
	if (x < 0)
		return -1;
	if (x == 0)
		return 0;
	return 1;
}

void __pspgl_copy_pixels(const void *srcbuf, int srcstride, int srcx, int srcy,
			 void *dstbuf, int dststride, int dstx, int dsty,
			 int width, int height, unsigned pixfmt)
{
	sendCommandi(CMD_COPY_SRC, (unsigned)srcbuf);
	sendCommandi(CMD_COPY_DST, (unsigned)dstbuf);

	if (srcstride < 0 || dststride < 0) {
		int sdy = sgni(srcstride);
		int ddy = sgni(dststride);

		srcstride = absi(srcstride);
		dststride = absi(dststride);

		sendCommandi(CMD_COPY_SRC_STRIDE, (((unsigned)srcbuf & 0xff000000) >> 8) | srcstride);
		sendCommandi(CMD_COPY_DST_STRIDE, (((unsigned)dstbuf & 0xff000000) >> 8) | dststride);

		sendCommandi(CMD_COPY_SIZE, ((1-1) << 10) | (width-1));

		while(height--) {
			sendCommandi(CMD_COPY_SRC_XY, (srcy << 10) | srcx);
			sendCommandi(CMD_COPY_DST_XY, (dsty << 10) | dstx);

			__pspgl_context_flush_pending_state_changes (pspgl_curctx,
								     CMD_COPY_SRC,
								     CMD_COPY_DST_STRIDE);
			__pspgl_context_flush_pending_state_changes (pspgl_curctx,
								     CMD_COPY_SRC_XY,
								     CMD_COPY_SIZE);
			sendCommandi(CMD_COPY_START, (pixfmt == GE_RGBA_8888));

			srcy += sdy;
			dsty += ddy;
		}
	} else {
		sendCommandi(CMD_COPY_SRC_STRIDE, (((unsigned)srcbuf & 0xff000000) >> 8) | srcstride);
		sendCommandi(CMD_COPY_DST_STRIDE, (((unsigned)dstbuf & 0xff000000) >> 8) | dststride);

		sendCommandi(CMD_COPY_SRC_XY, (srcy << 10) | srcx);
		sendCommandi(CMD_COPY_DST_XY, (dsty << 10) | dstx);

		sendCommandi(CMD_COPY_SIZE, ((height-1) << 10) | (width-1));

		__pspgl_context_flush_pending_state_changes (pspgl_curctx,
							     CMD_COPY_SRC,
							     CMD_COPY_DST_STRIDE);
		__pspgl_context_flush_pending_state_changes (pspgl_curctx,
							     CMD_COPY_SRC_XY,
							     CMD_COPY_SIZE);

		sendCommandiUncached(CMD_COPY_START, (pixfmt == GE_RGBA_8888));
	}
}

