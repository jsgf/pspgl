ARCH = psp-
CC = $(ARCH)gcc
PSP_INSTALL = ../tools/psp-install
RM = rm -f

PSPPATH := $(shell psp-config --pspsdk-path)
PSPGL_LFLAGS = -lGLU -lGL -lpspdebug -lpspge -lpspdisplay -lpspctrl -lpspsdk -lm -lc -lpspuser -lpsputility -lpspkernel -lpsprtc
CFLAGS = -g -Wall -O2 -MD -I$(PSPPATH)/include -I..
LFLAGS = -g -Wall -O2 -DMODULE_NAME="test-egl" psp-setup.c -L.. -L$(PSPPATH)/lib $(PSPGL_LFLAGS)

TARGET = pspGL-cube
OBJS = eglcube.o logo.o

BUILDDATE = $(shell date "+%Y/%m/%d %k:%M:%S")

PSPSDK=$(shell psp-config --pspsdk-path)


all: $(TARGET)

.c.o:
	$(CC) $(CFLAGS) -c $<

logo.o: logo.raw
	bin2o -i logo.raw logo.o logo

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LFLAGS) -o $@

install: all
	$(PSP_INSTALL) $(TARGET) --eboot-title="$(TARGET) $(BUILDDATE)" --eboot-icon="ball1.png"

clean:
	$(RM) *.d *.o *.a *.elf *.sfo EBOOT.PBP

-include $(wildcard *.d) dummy

