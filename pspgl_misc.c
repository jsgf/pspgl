#include <pspuser.h>
#include <pspge.h>

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "pspgl_internal.h"



void __pspgl_log (const char *fmt, ...)
{
	char buf [1024];
	va_list ap;
	int len;
	int log_fd;

	log_fd = sceIoOpen("ms0:/log.txt", PSP_O_CREAT | PSP_O_APPEND | PSP_O_WRONLY, 0644);

	if (log_fd >= 0) {
		va_start(ap, fmt);
		len = vsnprintf(buf, sizeof(buf), fmt, ap);
		va_end(ap);
		sceIoWrite(log_fd, buf, len);
		sceIoClose(log_fd);
	}
}


#define PSPGL_GE_DUMPFILE "ms0:/pspgl.ge"

static
void pspgl_ge_writefile (void *buf, unsigned long len)
{
	int fd = sceIoOpen(PSPGL_GE_DUMPFILE, PSP_O_CREAT | PSP_O_APPEND | PSP_O_WRONLY, 0644);
	sceIoWrite(fd, buf, len);
	sceIoClose(fd);
}


void __pspgl_ge_matrix_dump (void)
{
	unsigned long buf [2 + 12 * 16];
	int i;

	buf[0] = PSPGL_GE_DUMP_MATRIX;
	buf[1] = sizeof(buf);

	for (i=0; i<12; i++)
		sceGeGetMtx(i, (float *) &buf[2+i*16]);

	pspgl_ge_writefile(buf, sizeof(buf));
}


void __pspgl_ge_register_dump (void)
{
	unsigned long buf [2 + 256];
	int i;

	buf[0] = PSPGL_GE_DUMP_REGISTERS;
	buf[1] = sizeof(buf);

	for (i=0; i<256; i++)
		buf[2+i] = sceGeGetCmd(i);

	pspgl_ge_writefile(buf, sizeof(buf));
}


void __pspgl_dlist_dump (unsigned long *cmd_buf, unsigned long len)
{
	unsigned long header [3];
	int fd;

	header[0] = PSPGL_GE_DUMP_DLIST;
	header[1] = sizeof(header) + len * sizeof(cmd_buf[0]);
	header[2] = (unsigned long) cmd_buf;

	fd = sceIoOpen(PSPGL_GE_DUMPFILE, PSP_O_CREAT | PSP_O_APPEND | PSP_O_WRONLY, 0644);
	sceIoWrite(fd, header, sizeof(header));
	sceIoWrite(fd, (void *) cmd_buf, len * sizeof(cmd_buf[0]));
	sceIoClose(fd);
}


void __pspgl_vram_dump (void)
{
	unsigned long vram_start = (unsigned long) sceGeEdramGetAddr();
	unsigned long vram_size = (unsigned long) sceGeEdramGetSize();
	unsigned long header [4];
	unsigned char vram_copy [0x10000];
	int fd;
	int i;

	header[0] = PSPGL_GE_DUMP_VRAM;
	header[1] = sizeof(header) + vram_size;
	header[2] = vram_start;
	header[3] = vram_size;

	fd = sceIoOpen(PSPGL_GE_DUMPFILE, PSP_O_CREAT | PSP_O_APPEND | PSP_O_WRONLY, 0644);
	sceIoWrite(fd, header, sizeof(header));

	/* copy in blocks, direct writes from VRAM to file don't seem to work... */
	for (i=0; i<vram_size/sizeof(vram_copy); i++, vram_start+=sizeof(vram_copy)) {
		memcpy(vram_copy, (void *) vram_start, sizeof(vram_copy));
		sceIoWrite(fd, (void *) vram_copy, sizeof(vram_copy));
	}

	sceIoClose(fd);
}

