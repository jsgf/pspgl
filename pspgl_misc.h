#ifndef __pspgl_misc_h__
#define __pspgl_misc_h__

struct pspgl_dlist;

extern void __psp_log (const char *fmt, ...);


enum pspgl_dump_tag {
	PSPGL_GE_DUMP_MATRIX    = 1,
	PSPGL_GE_DUMP_REGISTERS = 2,
	PSPGL_GE_DUMP_DLIST     = 3
};

extern void __pspgl_ge_register_dump (void);
extern void __pspgl_ge_matrix_dump (void);
extern void __pspgl_dlist_dump (unsigned long *cmd_buf, unsigned long len);
extern void __pspgl_vram_dump (void);


/**
 *  Enable this to dump display lists, matrix and command registers to memory stick.
 *  Note: This will seriously slow down program exection, so be patient.
 *  The resulting file "ms0:/pspgl.ge" can easily get analyzed with the tools/decode_ge_dump utility.
 */
#if 0
#define pspgl_ge_register_dump() __pspgl_ge_register_dump()
#define pspgl_ge_matrix_dump() __pspgl_ge_matrix_dump()
#define pspgl_dlist_dump(buf,len) __pspgl_dlist_dump(buf,len)
#define pspgl_vram_dump() __pspgl_vram_dump()
#else
#define pspgl_ge_register_dump() do {} while (0)
#define pspgl_ge_matrix_dump() do {} while (0)
#define pspgl_dlist_dump(buf,len) do {} while (0)
#define pspgl_vram_dump() do {} while (0)
#endif

#if 0
#define psp_log(x...) do { __psp_log("%s (%d): ", __FUNCTION__, __LINE__); __psp_log(x); } while (0)
#else
#define psp_log(x...) do { } while (0)
#endif

#endif

