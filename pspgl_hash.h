#ifndef __hash_h__
#define __hash_h__


#define HASH_SIZE	(32)
#define HASH_NO_KEY	(~0)


struct hashtable {
	struct hashentry *buckets [HASH_SIZE];
	unsigned long maxkey;
};

extern void pspgl_hash_insert (struct hashtable *h, unsigned long key, void *value);
extern void* pspgl_hash_remove (struct hashtable *h, unsigned long key);
extern void* pspgl_hash_lookup (const struct hashtable *h, unsigned long key);

/* return unused key, HASH_NO_KEY on failure */
extern unsigned long pspgl_hash_uniquekey (const struct hashtable *h);

/* call func() for each entry, free entry thenafter. */
extern void pspgl_hash_foreach_free (struct hashtable *, void (*func) (void *data));


#endif

