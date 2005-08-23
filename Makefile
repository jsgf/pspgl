PSPPATH := $(shell psp-config --pspsdk-path)
PSPDIR  := $(shell psp-config --psp-prefix)
ARCH = psp-

CC = $(ARCH)gcc
AR = $(ARCH)ar
RANLIB = $(ARCH)ranlib
RM = rm -f
CFLAGS = -g -Wall -O2 -G0 -fsingle-precision-constant -I. -I $(PSPPATH)/include
LFLAGS = -g -Wall -O2 -G0 -L $(PSPPATH)/lib

DEPDIR = .deps

all: libGL.a libGLU.a libglut.a


libGL.a_OBJS = \
	eglChooseConfig.o \
	eglCreateContext.o \
	eglCreateWindowSurface.o \
	eglDestroyContext.o \
	eglDestroySurface.o \
	eglGetConfigAttrib.o \
	eglGetConfigs.o \
	eglGetError.o \
	eglGetDisplay.o \
	eglInitialize.o \
	eglMakeCurrent.o \
	eglQueryString.o \
	eglSwapBuffers.o \
	eglSwapInterval.o \
	eglTerminate.o \
	glAlphaFunc.o \
	glBegin.o \
	glBlendEquation.o \
	glBlendFunc.o \
	glClear.o \
	glClearColor.o \
	glClearDepthf.o \
	glColor.o \
	glColorPointer.o \
	glCullFace.o \
	glDepthFunc.o \
	glDepthRange.o \
	glDrawArrays.o \
	glDrawElements.o \
	glEnable.o \
	glEnableClientState.o \
	glEnd.o \
	glFinish.o \
	glFlush.o \
	glFog.o \
	glFrontFace.o \
	glFrustum.o \
	glGetError.o \
	glGetString.o \
	glInterleavedArrays.o \
	glLight.o \
	glLightModel.o \
	glLoadIdentity.o \
	glLoadMatrixf.o \
	glLogicOp.o \
	glMaterial.o \
	glMatrixMode.o \
	glMultMatrixf.o \
	glNormal.o \
	glNormalPointer.o \
	glOrtho.o \
	glPopMatrix.o \
	glPushMatrix.o \
	glPolygonOffset.o \
	glRotatef.o \
	glScalef.o \
	glScissor.o \
	glShadeModel.o \
	glStencilFunc.o \
	glStencilOp.o \
	glTexCoord.o \
	glTexCoordPointer.o \
	glTexEnv.o \
	glTexImage2D.o \
	glTexParameter.o \
	glTranslatef.o \
	glVertex.o \
	glVertexi.o \
	glVertexPointer.o \
	glViewport.o \
 	pspgl_dlist.o \
 	pspgl_ge_init.o \
 	pspgl_misc.o \
 	pspgl_varray.o \
 	pspgl_vidmem.o

libGLU.a_OBJS = \
	gluLookAt.o \
	gluPerspective.o

libglut.a_OBJS = \
	glut.o

%.a: $(DEPDIR) $(libGL.a_OBJS) $(libGLU.a_OBJS) $(libglut.a_OBJS)
	$(RM) $@
	$(AR) cru $@ $($@_OBJS)
	$(RANLIB) $@

$(DEPDIR):
	mkdir $(DEPDIR)

.c.o:
	$(CC) $(CFLAGS) -MD -MF $(DEPDIR)/$*.d -c $<

.S.o:
	$(CC) $(CFLAGS) -c $<

tar: clean
	( cd .. && tar cvfz pspgl-`date "+%Y-%m-%d"`.tar.gz pspgl --exclude "*.DS_Store" && cd - )

clean:
	$(RM) -rf *.o *.a $(DEPDIR)
	make -C tools clean
	make -C test-egl clean
	make -C test-glut clean

install: libGL.a
	mkdir -p $(PSPDIR)/include $(PSPDIR)/lib
	mkdir -p $(PSPDIR)/include/GL $(PSPDIR)/include/GLES
	cp GL/*.h $(PSPDIR)/include/GL
	cp GLES/*.h $(PSPDIR)/include/GLES
	cp libGL.a $(PSPDIR)/lib
	cp libGLU.a $(PSPDIR)/lib
	cp libglut.a $(PSPDIR)/lib

-include $(wildcard $(DEPDIR)/*.d) dummy

