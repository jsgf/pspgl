#include <pspkernel.h>
#include "sceGe.h"

#include <stdio.h>
#include <stdarg.h>

#include "pspgl_internal.h"



void __psp_log (const char *fmt, ...)
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

