	.set noreorder

#include "pspstub.s"

	STUB_START	"sceCtrl",0x40010000,0x000E0005
	STUB_FUNC	0x6A2774F3,sceCtrlSetSamplingCycle
	STUB_FUNC	0x02BAAD91,sceCtrlGetSamplingCycle
	STUB_FUNC	0x1F4011E6,sceCtrlSetSamplingMode
	STUB_FUNC	0xDA6B76A1,sceCtrlGetSamplingMode
	STUB_FUNC	0x3A622550,sceCtrlPeekBufferPositive
	STUB_FUNC	0xC152080A,sceCtrlPeekBufferNegative
	STUB_FUNC	0x1F803938,sceCtrlReadBufferPositive
	STUB_FUNC	0x60B81F86,sceCtrlReadBufferNegative
	STUB_FUNC	0xB1D0E5CD,sceCtrlPeekLatch
	STUB_FUNC	0x0B588501,sceCtrlReadLatch
	STUB_FUNC	0x348D99D4,sceCtrl_348D99D4
	STUB_FUNC	0xAF5960F3,sceCtrl_AF5960F3
	STUB_FUNC	0xA68FD260,sceCtrl_A68FD260
	STUB_FUNC	0x6841BE1A,sceCtrl_6841BE1A
	STUB_END
