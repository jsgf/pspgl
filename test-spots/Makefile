ARCH = psp-
CC = $(ARCH)gcc
AS = $(ARCH)as
STRIP = $(ARCH)strip
SIZE = $(ARCH)size
MKSFO = mksfo
PACK_PBP = pack-pbp
FIXUP_IMPORTS = psp-fixup-imports
RM = rm -f

PSPPATH := $(shell psp-config --pspsdk-path)
PSPGL_LFLAGS = -lglut -lGLU -lGL -lm -lpspdebug -lpspge -lpspdisplay -lpspctrl -lpspsdk -lpsplibc -lpspuser -lpspkernel -lpsprtc
CFLAGS = -g -Wall -O2 -MD -I.. -I$(PSPPATH)/include
LFLAGS = -g -Wall -O2 -DMODULE_NAME="$(TARGET)" psp-setup.c -L.. -L$(PSPPATH)/lib $(PSPGL_LFLAGS)

TARGET = spots
OBJS = simple.o firefox.o

EXTRA_TARGETS = EBOOT.PBP
BUILDDATE = $(shell date "+%Y/%m/%d %k:%M:%S")
PSP_EBOOT_TITLE = $(TARGET) $(BUILDDATE)
PSP_EBOOT_ICON = firefox.png
PSP_EBOOT_ICON1 = NULL
PSP_EBOOT_UNKPNG = NULL
PSP_EBOOT_PIC1 = NULL
PSP_EBOOT_SND0 = NULL
PSP_EBOOT_PSAR = NULL

PSPSDK=$(shell psp-config --pspsdk-path)


all: EBOOT.PBP


%.o: %.raw
	(sym=`echo $* | tr '-' '_'`; \
	 echo -e ".data\n.global $${sym}_start\n$${sym}_start:\n\t.incbin \"$<\"" | $(AS) -o $@)

%.raw: %.png
	convert -geometry 64x64 $< rgba:$@

.c.o:
	$(CC) $(CFLAGS) -c $<

$(TARGET).elf: $(OBJS) ../libGL.a ../libglut.a
	$(CC) $(OBJS) $(LFLAGS) -o $@

EBOOT.PBP: $(TARGET).elf
	$(FIXUP_IMPORTS) $(TARGET).elf
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

MOUNT=/mnt/stick
DIR=$(MOUNT)/psp/game

Linux-install: EBOOT.PBP
	while [ ! -d $(DIR) ]; do mount $(MOUNT); sleep 1; done
	mkdir -p $(DIR)/$(TARGET)
	mkdir -p $(DIR)/$(TARGET)%
	cp $(TARGET)_strip.elf $(DIR)/$(TARGET)/EBOOT.PBP
	cp EBOOT.PBP $(DIR)/$(TARGET)%/
	umount $(MOUNT)

clean:
	$(RM) *.d *.o *.a *.elf *.sfo EBOOT.PBP

-include $(wildcard *.d) dummy

