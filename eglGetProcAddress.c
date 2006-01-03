#include <string.h>
#include <stdlib.h>

#include "pspgl_internal.h"

struct pspgl_proc {
	const char *name;
	void	(*proc)();
};

static const struct pspgl_proc proctable[] = {
#include "pspgl_proctable.h"
};

static int cmp_procname(const void *a, const void *b)
{
	const char *name = a;
	const struct pspgl_proc *entry = b;

	return strcmp(name, entry->name);
}

GLAPI void (* APIENTRY eglGetProcAddress (const char *procname))()
{
	struct pspgl_proc *p;

	p = bsearch(procname, proctable, sizeof(proctable)/sizeof(*proctable), sizeof(proctable[0]),
		    cmp_procname);
	if (p != NULL)
		return p->proc;
	return NULL;
}

