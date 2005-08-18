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

OBJS = \
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
	gluPerspective.o \
	glut.o \
 	pspgl_dlist.o \
 	pspgl_ge_init.o \
 	pspgl_misc.o \
 	pspgl_varray.o \
 	pspgl_vidmem.o

libpspgl.a: $(DEPDIR) GL $(OBJS)
	$(RM) $@
	$(AR) cru $@ $(OBJS)
	$(RANLIB) $@

$(DEPDIR):
	mkdir $(DEPDIR)
GL:
	ln -s GLES GL

.c.o:
	$(CC) $(CFLAGS) -MD -MF $(DEPDIR)/$*.d -c $<

.S.o:
	$(CC) $(CFLAGS) -c $<

tar: clean
	( cd .. && tar cvfz pspgl-`date "+%Y-%m-%d"`.tar.gz pspgl --exclude "*.DS_Store" && cd - )

clean:
	$(RM) -rf *.o *.a $(DEPDIR) GL
	make -C tools clean
	make -C test-egl clean
	make -C test-glut clean

# Install headers to both GL and GLES directories.  This is to workaround
# platforms that don't support symbolic links.
install: libpspgl.a
	mkdir -p $(PSPDIR)/include $(PSPDIR)/lib
	mkdir -p $(PSPDIR)/include/GL $(PSPDIR)/include/GLES
	cp GLES/*.h $(PSPDIR)/include/GL
	cp GLES/*.h $(PSPDIR)/include/GLES
	cp libpspgl.a $(PSPDIR)/lib

-include $(wildcard $(DEPDIR)/*.d) dummy
