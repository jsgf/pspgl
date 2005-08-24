/*
XXX FIXME: CAN VANISH AS SOON THE PSPLIBC IS FIXED.
*/

#ifndef _DIRENT_H_
#define _DIRENT_H_

/* XXX FIXME: should be in sys/dirent.h for POSIX compliance */
/* declaration is not POSIX compliant, please review. */
struct dirent {
        ino_t d_ino;                    /* file number of entry */
        unsigned short d_reclen;            /* length of this record */
        unsigned char  d_type;              /* file type, see below */
        unsigned char  d_namlen;            /* length of string in d_name */
        char d_name[256 + 1];    /* name must be no longer than this */
};



/* structure describing an open directory. */
typedef struct {
	int	dd_fd;		/* file descriptor associated with directory */
	long	dd_loc;		/* offset in current buffer */
	long	dd_size;	/* amount of data returned by getdirentries */
	char	*dd_buf;	/* data buffer */
	int	dd_len;		/* size of data buffer */
	long	dd_seek;	/* magic cookie returned by getdirentries */
	long	dd_rewind;	/* magic cookie for rewinding */
	int	dd_flags;	/* flags for readdir */
	struct _telldir *dd_td;	/* telldir position recording */
} DIR;

#ifndef _POSIX_C_SOURCE
/* definitions for library routines operating on directories. */
#define	DIRBLKSIZ	1024

#define	dirfd(dirp)	((dirp)->dd_fd)

/* flags for opendir2 */
#define DTF_HIDEW	0x0001	/* hide whiteout entries */
#define DTF_NODUP	0x0002	/* don't return duplicate names */
#define DTF_REWIND	0x0004	/* rewind after reading union stack */
#define __DTF_READALL	0x0008	/* everything has been read */

#endif /* ! _POSIX_C_SOURCE */

#include <sys/cdefs.h>

int closedir(DIR *);
DIR *opendir(const char *);
struct dirent *readdir(DIR *);
void rewinddir(DIR *);
void seekdir(DIR *, long);
long telldir(DIR *);

#endif /* !_DIRENT_H_ */

