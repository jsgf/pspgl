PSPPATH := $(shell psp-config --pspsdk-path)
ARCH = psp-

CC = $(ARCH)gcc
AR = $(ARCH)ar
RANLIB = $(ARCH)ranlib
RM = rm -f
CFLAGS = -g -Wall -O2 -G0 -fsingle-precision-constant -I. -I $(PSPPATH)/include
LFLAGS = -g -Wall -O2 -G0 -L $(PSPPATH)/lib

DEPDIR = .deps


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
	glArrayElement.o \
	glBegin.o \
	glBlendEquation.o \
	glBlendFunc.o \
	glClear.o \
	glClearColor.o \
	glClearDepth.o \
	glClearDepthf.o \
	glClearStencil.o \
	glColor.o \
	glColorMask.o \
	glColorPointer.o \
	glCullFace.o \
	glDepthFunc.o \
	glDepthMask.o \
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
	glFrustumf.o \
	glFrustum.o \
	glGetFloatv.o \
	glGetIntegerv.o \
	glGetError.o \
	glGetString.o \
	glInterleavedArrays.o \
	glLight.o \
	glLightModel.o \
	glLineWidth.o \
	glLoadIdentity.o \
	glLoadMatrixf.o \
	glLogicOp.o \
	glMaterial.o \
	glMatrixMode.o \
	glMultMatrixf.o \
	glNormal.o \
	glNormalPointer.o \
	glOrtho.o \
	glOrthof.o \
	glPopMatrix.o \
	glPushMatrix.o \
	glPolygonMode.o \
	glPolygonOffset.o \
	glRotatef.o \
	glScalef.o \
	glScissor.o \
	glShadeModel.o \
	glStencilFunc.o \
	glStencilMask.o \
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
	gluLookAtf.o \
	gluPerspective.o \
	gluPerspectivef.o

libglut.a_OBJS = \
	glut.o

all: $(DEPDIR) $(libGL.a_OBJS) $(libGLU.a_OBJS) $(libglut.a_OBJS) libGL.a libGLU.a libglut.a


%.a: $(libGL.a_OBJS) $(libGLU.a_OBJS) $(libglut.a_OBJS)
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

install: all
	mkdir -p $(PSPPATH)/include $(PSPPATH)/lib
	mkdir -p $(PSPPATH)/include/GL $(PSPPATH)/include/GLES
	cp GL/*.h $(PSPPATH)/include/GL
	cp GLES/*.h $(PSPPATH)/include/GLES
	cp libGL.a $(PSPPATH)/lib
	cp libGLU.a $(PSPPATH)/lib
	cp libglut.a $(PSPPATH)/lib

-include $(wildcard $(DEPDIR)/*.d) dummy

