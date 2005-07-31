ARCH = psp-
CC = $(ARCH)gcc
STRIP = $(ARCH)strip
SIZE = $(ARCH)size
MKSFO = mksfo
PACK_PBP = pack-pbp
RM = rm -f

PSPPATH := $(shell psp-config --pspsdk-path)
PSPGL_LFLAGS = -lpspgl -lm -lpspdebug -lpspge -lpspdisplay -lpspctrl -lpspsdk -lpsplibc -lpspuser -lpspkernel
CFLAGS = -g -Wall -O2 -MD -I$(PSPPATH)/include -I..
LFLAGS = -g -Wall -O2 -L$(PSPPATH)/lib -L.. $(PSPGL_LFLAGS)

TARGET = glut-simple
OBJS = simple.o

EXTRA_TARGETS = EBOOT.PBP
BUILDDATE = $(shell date "+%Y/%m/%d %k:%M:%S")
PSP_EBOOT_TITLE = $(TARGET) $(BUILDDATE)
PSP_EBOOT_ICON = ball1.png
PSP_EBOOT_ICON1 = NULL
PSP_EBOOT_UNKPNG = NULL
PSP_EBOOT_PIC1 = NULL
PSP_EBOOT_SND0 = NULL
PSP_EBOOT_PSAR = NULL

PSPSDK=$(shell psp-config --pspsdk-path)


all: EBOOT.PBP

.c.o:
	$(CC) $(CFLAGS) -c $<

$(TARGET).elf: $(OBJS)
	$(CC) $(OBJS) $(LFLAGS) -o $@

EBOOT.PBP: $(TARGET).elf
	$(MKSFO) '$(PSP_EBOOT_TITLE)' $(TARGET).sfo
	$(STRIP) $(TARGET).elf -o $(TARGET)_strip.elf
	@echo -------------------------------------------------------------------------------------
	$(SIZE) $(TARGET)_strip.elf
	@echo -------------------------------------------------------------------------------------
	$(PACK_PBP) EBOOT.PBP $(TARGET).sfo $(PSP_EBOOT_ICON)  \
		$(PSP_EBOOT_ICON1) $(PSP_EBOOT_UNKPNG) $(PSP_EBOOT_PIC1)  \
		$(PSP_EBOOT_SND0)  $(TARGET)_strip.elf $(PSP_EBOOT_PSAR)

install: all $(shell uname)-install

Darwin-install:
	@test -d "/Volumes/PSP STICK/" || \
		echo =========== PSP not connected, not installing =======================================
	@test -d "/Volumes/PSP STICK/"
	@echo ============= image done, now install on PSP ========================================
	rm -f "/Volumes/PSP STICK/log.txt"
	rm -f "/Volumes/PSP STICK/pspgl.ge"
	rm -rf "/Volumes/PSP STICK/PSP/GAME/$(TARGET)"
	mkdir "/Volumes/PSP STICK/PSP/GAME/$(TARGET)"
	cp -v EBOOT.PBP "/Volumes/PSP STICK/PSP/GAME/$(TARGET)/"
	sync

clean:
	$(RM) *.d *.o *.a *.elf *.sfo EBOOT.PBP

-include $(wildcard *.d) dummy

