//!\file fio.c \brief file utilities and macros

#include <errno.h>

#if WIN32||_WIN64
extern J zseek(I d,J j,I f);
extern I ftrunc(FILE*d,UJ n);
#else
#include <sys/types.h>
#include <unistd.h>
extern J zseek(FILE*d,J j,I f);
extern I ftrunc(FILE*d,UJ n);
#endif

#define errnoargs strerror(errno),errno
#define xfopen(fd,fname,mode,throws) \
	fd=fopen(fname,mode); \
	X(fd==NULL,T(WARN, "fopen(%s) %s (%d)", fname, errnoargs), throws)

extern UJ fsize(FILE *fp);

//:~
