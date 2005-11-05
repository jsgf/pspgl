ARCH = psp-
CC = $(ARCH)gcc
PSP_INSTALL = ../tools/psp-install
RM = rm -f

PSPPATH := $(shell psp-config --pspsdk-path)
PSPGL_LFLAGS = -lglut -lGLU -lGL -lm -lpspdebug -lpspge -lpspdisplay -lpspctrl -lpspsdk -lpsplibc -lpspuser -lpspkernel -lpsprtc
CFLAGS = -g -Wall -O2 -MD -I$(PSPPATH)/include -I..
LFLAGS = -g -Wall -O2 -DMODULE_NAME="test-glut" psp-setup.c -L.. -L$(PSPPATH)/lib $(PSPGL_LFLAGS)

TARGET = glut-simple
OBJS = simple.o

BUILDDATE = $(shell date "+%Y/%m/%d %k:%M:%S")

PSPSDK=$(shell psp-config --pspsdk-path)


all: $(TARGET)

.c.o:
	$(CC) $(CFLAGS) -c $<

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LFLAGS) -o $@

install: all
	$(PSP_INSTALL) $(TARGET) --eboot-title="$(TARGET) $(BUILDDATE)" --eboot-icon="ball1.png"

clean:
	$(RM) *.d *.o $(TARGET) 

-include $(wildcard *.d) dummy

