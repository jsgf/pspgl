#include <pspkernel.h>
#include <stdlib.h>
#include "pspgl_internal.h"


PSP_MODULE_INFO("pspEGL", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);


static
int exit_callback (int arg1, int arg2, void *common)
{
	eglTerminate(0);
	return 0;
}


static
int update_thread (SceSize args, void *argp)
{
	int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);
	sceKernelSleepThreadCB();
	return 0;
}


EGLBoolean eglInitialize (EGLDisplay dpy, EGLint *major, EGLint *minor)
{
	static int initialized = 0;

	psp_log("\n\n=============== pspGL, build " __DATE__ ", " __TIME__ " ===============\n");

	if (!initialized) {
		int id;
		
		if ((id = sceKernelCreateThread("update_thread", update_thread, 0x11, 0xFA0, 0, 0)) >= 0)
			sceKernelStartThread(id, 0, 0);

		initialized = 1;
	}

	if (major)
		*major = 1;
	if (minor)
		*minor = 1;

	return EGL_TRUE;
}
