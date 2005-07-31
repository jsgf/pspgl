PSPPATH := $(shell psp-config --pspsdk-path)
ARCH = psp-

CC = $(ARCH)gcc
AR = $(ARCH)ar
RANLIB = $(ARCH)ranlib
RM = rm -f
CFLAGS = -g -Wall -O0 -MD -I. -I $(PSPPATH)/include
LFLAGS = -g -Wall -O0 -L $(PSPPATH)/lib

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
 	pspgl_vidmem.o \
	sceCtrl.o \
	sceDisplay.o \
	sceGe.o

libpspgl.a: $(OBJS)
	$(RM) $@
	$(AR) cru $@ $(OBJS)
	$(RANLIB) $@

.c.o:
	$(CC) $(CFLAGS) -c $<

.S.o:
	$(CC) $(CFLAGS) -c $<

tar: clean
	( cd .. && tar cvfz pspgl-`date "+%Y-%m-%d"`.tar.gz pspgl --exclude "*.DS_Store" && cd - )

clean:
	$(RM) *.d *.o *.a
	make -C tools clean
	make -C test-egl clean
	make -C test-glut clean

-include $(wildcard *.d) dummy

