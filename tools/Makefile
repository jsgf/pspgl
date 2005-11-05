CC = gcc
RM = rm -f
CFLAGS = -g -O0 -Wall -D__LITTLE_ENDIAN__

TARGETS = decode_ge_dump

all: $(TARGETS)

decode_ge_dump: decode_ge_dump.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) $(TARGETS)

