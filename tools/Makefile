CC = gcc
RM = rm -f
CFLAGS = -g -O0 -Wall -MD

TARGETS = decode_ge_dump decode_vram_dump

all: $(TARGETS)

.c: 
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) $(TARGETS)

