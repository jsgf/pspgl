#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>


static inline
uint32_t swap32 (uint32_t x)
{
	return  ((x >> 24) & 0x000000ff) |
		((x >> 8)  & 0x0000ff00) |
		((x << 8)  & 0x00ff0000) |
		((x << 24) & 0xff000000);
}


#if defined(__BIG_ENDIAN__)
#define be32_to_cpu(x) (x)
#define le32_to_cpu(x) swap32(x)
#elif defined(__LITTLE_ENDIAN__)
#define le32_to_cpu(x) (x)
#define be32_to_cpu(x) swap32(x)
#else
#error unknown endianess!!
#endif


enum pspgl_dump_tag {
        PSPGL_GE_DUMP_MATRIX    = 1,
        PSPGL_GE_DUMP_REGISTERS = 2,
        PSPGL_GE_DUMP_DLIST     = 3
};


#define DUMP(x...) do { printf(x); } while (0)


static
float float32 (uint32_t i)
{
	i <<= 8;
	return *(float *) &i;
}


static const char *test_function [] = {
	"NEVER", "ALWAYS", "EQUAL", "NOTEQUAL", "LESS", "LEQUAL", "GREATER", "GEQUAL"
};


static
void process_insn (uint32_t insn)
{
	uint8_t opcode = insn >> 24;
	uint32_t arg = insn & 0x00ffffff;

	switch (opcode) {
	case 0x00:
		DUMP("NOP");
		break;
	case 0x01:
		DUMP("Vertex Address (BASE) + 0x%08x", arg);
		break;
	case 0x02:
		DUMP("Index Address (BASE) + 0x%08x", arg);
		break;
	case 0x04:
		DUMP("Primitive Kick: %d vertices, type %d", arg & 0xffff, arg >> 16);
		break;
	case 0x08:
		DUMP("Jump to Address (BASE) + 0x%08x", arg);
		break;
	case 0x0c:
		DUMP("END");
		break;
	case 0x0e:
		DUMP("Raise Signal Interrupt");
		break;
	case 0x0f:
		DUMP("FINISH");
		break;
	case 0x10:
		DUMP("BASE Address 0x%08x", (arg << 8) & 0xff000000);
		break;
	case 0x12:
		DUMP("Vertex Type 0x%08x", arg);
		break;
	case 0x13:
		DUMP("Offset Address 0x%08x", arg);
		break;
	case 0x14:
		DUMP("Origin Address 0x%08x", arg);
		break;
	case 0x15:
		DUMP("Draw Region Start x %d, y %d", arg & 0x3ff, arg >> 10);
		break;
	case 0x16:
		DUMP("Draw Region End x %d, y %d", arg & 0x3ff, arg >> 10);
		break;
	case 0x17:
		DUMP("Enable Lighting = %d", arg);
		break;
	case 0x18:
		DUMP("Enable Light 0 = %d", arg);
		break;
	case 0x19:
		DUMP("Enable Light 1 = %d", arg);
		break;
	case 0x1a:
		DUMP("Enable Light 2 = %d", arg);
		break;
	case 0x1b:
		DUMP("Enable Light 3 = %d", arg);
		break;
	case 0x1c:
		DUMP("Enable Clipping = %d", arg);
		break;
	case 0x1d:
		DUMP("Enable Culling = %d", arg);
		break;
	case 0x1e:
		DUMP("Enable Texture2D = %d", arg);
		break;
	case 0x1f:
		DUMP("Enable Fog = %d", arg);
		break;
	case 0x20:
		DUMP("Enable Dither = %d", arg);
		break;
	case 0x21:
		DUMP("Enable Alpha Blending = %d", arg);
		break;
	case 0x22:
		DUMP("Enable Alpha Test = %d", arg);
		break;
	case 0x23:
		DUMP("Enable Depth Test = %d", arg);
		break;
	case 0x24:
		DUMP("Enable Stencil Test = %d", arg);
		break;
	case 0x25:
		DUMP("Enable Antialiasing = %d", arg);
		break;
	case 0x26:
		DUMP("Enable Patch Culling(1) = %d", arg);
		break;
	case 0x27:
		DUMP("Enable Patch Culling(2) = %d", arg);
		break;
	case 0x28:
		DUMP("Enable Color LogicOp = %d", arg);
		break;
	case 0x2a:
		DUMP("Bone Matrix Offset = %d", arg);
		break;
	case 0x2b:
		DUMP("Bone Matrix Value = %1.5f", float32(arg));
		break;
	case 0x2c ... 0x33:
		DUMP("Morph Weight %d = %1.5f", opcode - 0x2c, float32(arg));
		break;
	case 0x36:
		DUMP("Patch Divide (S/T) %d/%d", arg & 0xff, arg >> 8);
		break;
	case 0x37:
		DUMP("Patch Primitive Type %d (%s)", arg, arg == 0 ? "TRIANGLES" : arg == 1 ? "LINES" : arg == 2 ? "POINTS" : "???");
		break;
	case 0x38:
		DUMP("Patch Front Face Orientation = %d (%s)", arg, arg == 0 ? "CW" : arg == 1 ? "CCW" : "???");
		break;
	case 0x3a:
		DUMP("World Matrix Select = %d", arg);
		break;
	case 0x3b:
		DUMP("World Matrix Upload %1.5f", float32(arg));
		break;
	case 0x3c:
		DUMP("View Matrix Select = %d", arg);
		break;
	case 0x3d:
		DUMP("View Matrix Upload %1.5f", float32(arg));
		break;
	case 0x3e:
		DUMP("Projection Matrix Select = %d", arg);
		break;
	case 0x3f:
		DUMP("Projection Matrix Upload %1.5f", float32(arg));
		break;
	case 0x40:
		DUMP("Texture Matrix Select = %d", arg);
		break;
	case 0x41:
		DUMP("Texture Matrix Upload %1.5f", float32(arg));
		break;
	case 0x42:
		DUMP("Viewport Size X %1.5f", float32(arg));
		break;
	case 0x43:
		DUMP("Viewport Size Y %1.5f", float32(arg));
		break;
	case 0x44:
		DUMP("Viewport Size Z / Depth Range %1.5f", float32(arg));
		break;
	case 0x45:
		DUMP("Viewport Offset X %1.5f", float32(arg));
		break;
	case 0x46:
		DUMP("Viewport Offset Y %1.5f", float32(arg));
		break;
	case 0x47:
		DUMP("Viewport Offset Z / Depth Offset %1.5f", float32(arg));
		break;
	case 0x48:
		DUMP("Texture Scale S %1.5f", float32(arg));
		break;
	case 0x49:
		DUMP("Texture Scale T %1.5f", float32(arg));
		break;
	case 0x4a:
		DUMP("Texture Offset S %1.5f", float32(arg));
		break;
	case 0x4b:
		DUMP("Texture Offset T %1.5f", float32(arg));
		break;
	case 0x4c:
		DUMP("Viewport/Screen Offset X %d", arg >> 4);
		break;
	case 0x4d:
		DUMP("Viewport/Screen Offset Y %d", arg >> 4);
		break;
	case 0x50:
		DUMP("Shading = %d (%s)", arg, arg == 0 ? "FLAT" : arg == 1 ? "SMOOTH" : "???");
		break;
	case 0x51:
		DUMP("Enable Normal Reverse = %d", arg);
		break;
	case 0x53:
		DUMP("Color Material 0x%06x", arg);
		break;
	case 0x5b:
		DUMP("Specular Power %1.5f", float32(arg));
		break;
	case 0x9b:
		DUMP("Front Face Orientation = %d (%s)", arg, arg == 0 ? "CW" : arg == 1 ? "CCW" : "???");
		break;
	case 0x9c:
		DUMP("Frame Buffer Pointer [23-0] 0x%08x", arg);
		break;
	case 0x9d:
		DUMP("Frame Buffer Width %d. Frame Buffer Pointer [32-24] 0x%08x", arg & 0xffff, (arg << 8) & 0xff000000);
		break;
	case 0x9e:
		DUMP("Depth Buffer Pointer [23-0] 0x%08x", arg);
		break;
	case 0x9f:
		DUMP("Depth Buffer Width %d. Frame Buffer Pointer [32-24] 0x%08x", arg & 0xffff, (arg << 8) & 0xff000000);
		break;
	case 0xd2:
		DUMP("Pixel Format %d (%s)", arg, arg == 0 ? "RGB565" : arg == 1 ? "RGBA551" : arg == 2 ? "RGBA4444" : arg == 3 ? "RGBA8888" : "???");
		break;
	case 0xd3:
		DUMP("Clear Enable = %d, Flags (%s|%s|%s)", (arg & 1),
			(arg & (1 << 8)) ? "CLEAR_COLOR" : "-",
			(arg & (1 << 9)) ? "CLEAR_ALPHA/STENCIL" : "-",
			(arg & (1 << 10)) ? "CLEAR_DEPTH" : "-");
		break;
	case 0xd4:
		DUMP("Scissor Lower Left %d/%d", arg & 0x3ff, arg >> 10);
		break;
	case 0xd5:
		DUMP("Scissor Upper Right %d/%d", arg & 0x3ff, arg >> 10);
		break;
	case 0xd6:
		DUMP("Near Depth Plane %d / %f", arg, float32(arg));
		break;
	case 0xd7:
		DUMP("Far Depth Plane %d / %f", arg, float32(arg));
		break;
	case 0xde:
		DUMP("Depth Test Function %d (%s)", arg, (arg < sizeof(test_function)/sizeof(test_function[0])) ? test_function[arg] : "???");
		break;
	case 0xe2 ... 0xe5:
		DUMP("Dither Matrix %d", opcode - 0xe2);
		break;
	default:
		;
	}
}


static
void process_chunk (uint32_t tag, uint32_t *buf, unsigned long len)
{
	uint32_t adr, i;

	printf("\ngot chunk (tag 0x%08x, len %lu bytes)\n", tag, len);

	switch (tag) {
	case PSPGL_GE_DUMP_MATRIX:
		for (i=0; i<12; i++) {
			uint32_t *m = &buf[16*i];
			int j, k;
			int rows = (i==10) ? 4 : 3;
			printf("\n");
			for (j=0; j<rows; j++) {
				printf("matrix %d [ ", i);
				for (k=0; k<4; k++) {
					printf("% 2.4f ", float32(le32_to_cpu(m[rows*k+j])));
				}
				printf("]\n");
			}
		}
		break;
	case PSPGL_GE_DUMP_DLIST:
		adr = le32_to_cpu(buf[0]);
		len -= 4;
		buf++;
		for (i=0; i<len/4; i++) {
			uint32_t insn = le32_to_cpu(buf[i]);
			printf("dlist 0x%08x: 0x%08x    ", adr+4*i, insn);
			process_insn(insn);
			printf("\n");
		}
		break;
	case PSPGL_GE_DUMP_REGISTERS:
		for (i=0; i<len/4; i++)
			printf("reg 0x%02x: 0x%08x\n", i, le32_to_cpu(buf[i]));
		break;
	default:
		fprintf(stderr, "**** unknown tag! ****\n");
	}
}


int main (int argc, char **argv)
{
	unsigned long flen, fpos;
	uint32_t *fptr;
	int fd;

	if (argc != 2) {
		fprintf(stderr, "\n\tusage: %s <dump.ge>\n\n", argv[0]);
		return -1;
	}

	fd = open(argv[1], O_RDONLY, 0);
	flen = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	fptr = (uint32_t*) mmap(NULL, flen, PROT_READ, MAP_PRIVATE, fd, 0);

	fpos = 0;
	flen /= 4;

	while (fpos<flen-2) {
		uint32_t tag = le32_to_cpu(fptr[fpos]);
		uint32_t len = le32_to_cpu(fptr[fpos+1]);

		if (fpos+len/4 > flen) {
			fprintf(stderr, "**** unexpected end of file! ****\n");
			break;
		}

		if (len == 0)
			break;

		process_chunk(tag, &fptr[fpos+2], len-2*4);
		fpos += len/4;
	}

	munmap((void*) fptr, flen);
	close(fd);

	return 0;
}
