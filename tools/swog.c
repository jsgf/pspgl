#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

/* 
   0  -> 0
   16 -> 32
   32 -> 64
   48 -> 96...

   256 -> 16
   

   8 7 6 5 4 3 2 1 0

   vvvvvvvvvvvvvvvvv

   7 6 5 4 9 3 2 1 0
 */

unsigned swoz(unsigned x)
{
	return ((x & 0x100) >> 4) | ((x & 0xf0) << 1) | x & 0xf;
}


int main(int argc, char **argv)
{
	int x, y;
	int in = open(argv[1], O_RDONLY);
	int out = open(argv[2], O_RDWR | O_TRUNC | O_CREAT, 0660);
	unsigned size;
	const unsigned char *p;
	unsigned char *dest;

	if (in == -1) {
		perror(argv[1]);
		return 1;
	}
	if (out == -1) {
		perror(argv[2]);
		return 1;
	}

	size = lseek(in, 0, SEEK_END);
	p = mmap(0, size, PROT_READ, MAP_PRIVATE, in, 0);
	if (ftruncate(out, size) == -1) {
		perror("ftruncate");
		return 1;
	}
	dest = mmap(0, size, PROT_READ|PROT_WRITE, MAP_SHARED, out, 0);


	for(y = 0; y < size/512; y++) {
		for(x = 0; x < 512; x += 16)
			memcpy(&dest[y * 512 + (x)], &p[y * 512 + swoz(x)], 16);
	}
	close(in);
	close(out);
	return 0;
}
		
