//!\file fio.c \brief file utilities and macros

#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#if WIN32||_WIN64
ext J zseek(I d, J j, I f);
ext I ftrunc(FILE* d, UJ n);
#else
#include <sys/types.h>
#include <unistd.h>
ext J zseek(FILE* d, J j, I f);
ext I ftrunc(FILE* d, UJ n);
#endif

#define errnoargs strerror(errno),errno
#define xfopen(fd,fname,mode,throws) \
	fd=fopen(fname,mode); \
	X(fd==NULL, T(WARN, "fopen(%s) %s (%d)", fname, errnoargs), throws)

ext UJ fsize(FILE*fp);
ext  C fexist(S fname);

ext V* xmmap(S fname);
ext  V xmunmap(V*map,sz size);

//:~
