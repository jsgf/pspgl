#include <pspdebug.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include "../pspgl_codegen.h"


/* set all VFPU regs zero */
void vfpu_init (void)
{
        __asm__ volatile (
		cgen_asm(vmzero_q(Q_M000))	/* access register array as matrices for speed */
		cgen_asm(vmzero_q(Q_M100))
		cgen_asm(vmzero_q(Q_M200))
		cgen_asm(vmzero_q(Q_M300))
		cgen_asm(vmzero_q(Q_M400))
		cgen_asm(vmzero_q(Q_M500))
		cgen_asm(vmzero_q(Q_M600))
		cgen_asm(vmzero_q(Q_M700))
	);
}

/* initialize VFPU test pattern */
void vfpu_init1 (void)
{
        __asm__ volatile (
		cgen_asm(vone_s(S_S000))
		cgen_asm(vadd_s(S_S001, S_S000, S_S000))
		cgen_asm(vadd_s(S_S002, S_S001, S_S000))
		cgen_asm(vadd_s(S_S003, S_S002, S_S000))     /* C000 = ( 1  2  3  4) */
		cgen_asm(vone_q(Q_C730))                     /* C730 = ( 1  1  1  1) */
		cgen_asm(vscl_q(Q_C730, Q_C730, S_S003))     /* C730 = ( 4  4  4  4) */
		cgen_asm(vadd_q(Q_C010, Q_C000, Q_C730))     /* C010 = ( 5  6  7  8) */
		cgen_asm(vadd_q(Q_C020, Q_C010, Q_C730))     /* C020 = ( 9 10 11 12) */
		cgen_asm(vadd_q(Q_C030, Q_C020, Q_C730))     /* C030 = (13 14 15 16) */
		cgen_asm(vadd_q(Q_C100, Q_C030, Q_C730))     /* C100 = (17 18 19 20) */
		cgen_asm(vadd_q(Q_C110, Q_C100, Q_C730))     /* C110 = (21 22 23 24) */
		cgen_asm(vadd_q(Q_C120, Q_C110, Q_C730))     /* ... */
		cgen_asm(vadd_q(Q_C130, Q_C120, Q_C730))
		cgen_asm(vmmov_q(Q_M200, Q_M000))            /* M2 = M0 */
		cgen_asm(vmmov_q(Q_M300, Q_M100))            /* M3 = M1 */
		cgen_asm(vmzero_q(Q_M400))                   /* M4 = M5 = M6 = M7 = 0.0f */
		cgen_asm(vmzero_q(Q_M500))
		cgen_asm(vmzero_q(Q_M600))
		cgen_asm(vmzero_q(Q_M700))
	);
}


void vfpu_save_regs (float vfpu_regs [32][4])
{
        register void *ptr __asm__ ("a0") = vfpu_regs;
        __asm__ volatile (
		cgen_asm(sv_q(Q_C000, 0 * 16, R_a0, 0))
		cgen_asm(sv_q(Q_C010, 1 * 16, R_a0, 0))
		cgen_asm(sv_q(Q_C020, 2 * 16, R_a0, 0))
                cgen_asm(sv_q(Q_C030, 3 * 16, R_a0, 0))
                cgen_asm(sv_q(Q_C100, 4 * 16, R_a0, 0))
                cgen_asm(sv_q(Q_C110, 5 * 16, R_a0, 0))
                cgen_asm(sv_q(Q_C120, 6 * 16, R_a0, 0))
                cgen_asm(sv_q(Q_C130, 7 * 16, R_a0, 0))
                cgen_asm(sv_q(Q_C200, 8 * 16, R_a0, 0))
                cgen_asm(sv_q(Q_C210, 9 * 16, R_a0, 0))
                cgen_asm(sv_q(Q_C220, 10 * 16, R_a0, 0))
                cgen_asm(sv_q(Q_C230, 11 * 16, R_a0, 0))
                cgen_asm(sv_q(Q_C300, 12 * 16, R_a0, 0))
                cgen_asm(sv_q(Q_C310, 13 * 16, R_a0, 0))
                cgen_asm(sv_q(Q_C320, 14 * 16, R_a0, 0))
                cgen_asm(sv_q(Q_C330, 15 * 16, R_a0, 0))
                cgen_asm(sv_q(Q_C400, 16 * 16, R_a0, 0))
                cgen_asm(sv_q(Q_C410, 17 * 16, R_a0, 0))
                cgen_asm(sv_q(Q_C420, 18 * 16, R_a0, 0))
                cgen_asm(sv_q(Q_C430, 19 * 16, R_a0, 0))
                cgen_asm(sv_q(Q_C500, 20 * 16, R_a0, 0))
                cgen_asm(sv_q(Q_C510, 21 * 16, R_a0, 0))
                cgen_asm(sv_q(Q_C520, 22 * 16, R_a0, 0))
                cgen_asm(sv_q(Q_C530, 23 * 16, R_a0, 0))
                cgen_asm(sv_q(Q_C600, 24 * 16, R_a0, 0))
                cgen_asm(sv_q(Q_C610, 25 * 16, R_a0, 0))
                cgen_asm(sv_q(Q_C620, 26 * 16, R_a0, 0))
                cgen_asm(sv_q(Q_C630, 27 * 16, R_a0, 0))
                cgen_asm(sv_q(Q_C700, 28 * 16, R_a0, 0))
                cgen_asm(sv_q(Q_C710, 29 * 16, R_a0, 0))
                cgen_asm(sv_q(Q_C720, 30 * 16, R_a0, 0))
                cgen_asm(sv_q(Q_C730, 31 * 16, R_a0, 0))
		: "=r"(ptr) : "r"(ptr) : "memory");
}


void vfpu_diff (float r1 [32][4], float r2 [32][4])
{
        int i, j;

        for (i=0; i<32; i++) {
                for (j=0; j<4; j++) {
			if (r1[i][j] != r2[i][j])
				break;
		}
		if (j<4)
			pspDebugScreenPrintf("-%2i% 7.3f% 7.3f% 7.3f% 7.3f %08x%08x%08x%08x\n", 
						i,
						r1[i][0], r1[i][1], r1[i][2], r1[i][3],
						(unsigned) r1[i][0],
						(unsigned) r1[i][1],
						(unsigned) r1[i][2],
						(unsigned) r1[i][3]);
	}

        for (i=0; i<32; i++) {
                for (j=0; j<4; j++) {
			if (r1[i][j] != r2[i][j])
				break;
		}
		if (j<4)
			pspDebugScreenPrintf("+%2i% 7.3f% 7.3f% 7.3f% 7.3f %08x%08x%08x%08x\n", 
						i,
						r2[i][0], r2[i][1], r2[i][2], r2[i][3],
						(unsigned) r2[i][0],
						(unsigned) r2[i][1],
						(unsigned) r2[i][2],
						(unsigned) r2[i][3]);
	}
	pspDebugScreenPrintf("\n");
}


static float vfpu_regs0 [32][4] __attribute__((aligned(16)));
static float vfpu_regs1 [32][4] __attribute__((aligned(16)));

/**
 *  ok... this function is the place to actually try the behaviour of some yet-unknown instructions.
 */
void vfpu_testcase (void)
{
	__asm__(cgen_asm(vone_q(Q_C000)));
	__asm__(cgen_asm(vadd_q(Q_C000, Q_C000, Q_C000)));
	__asm__(cgen_asm(vadd_q(Q_C000, Q_C000, Q_C000)));
	__asm__(cgen_asm(vadd_q(Q_C000, Q_C000, Q_C000)));
	__asm__(cgen_asm(vt4444_q(Q_C010, Q_C000)));
}


int main (int argc, char **argv)
{
	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_DIGITAL);

	pspDebugScreenInit();
	pspDebugScreenPrintf("VFPU test  --  vfpu_regs0 = %p, vfpu_regs1 = %p\n\n", vfpu_regs0, vfpu_regs1);
	pspDebugScreenPrintf("press O (run VFPU test), Square (trap into breakpoint), X to exit\n\n");

	vfpu_init();
	//vfpu_init1();

	while (1) {
		SceCtrlData pad;

		sceCtrlReadBufferPositive(&pad, 1);

		if (pad.Buttons & PSP_CTRL_CIRCLE) {
			vfpu_save_regs(vfpu_regs0);
			vfpu_testcase();
			vfpu_save_regs(vfpu_regs1);
			vfpu_diff(vfpu_regs0, vfpu_regs1);
		}

		if (pad.Buttons & PSP_CTRL_SQUARE)
			asm("break\n"); /* Cause a break exception, to check that the exception handler works... */

		if (pad.Buttons & PSP_CTRL_CROSS)
			break;

		sceDisplayWaitVblankStart();
	}

	return 0;
}
