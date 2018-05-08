#if WIN32||_WIN64
extern J zseek(I d,J j,I f);
extern I ftrunc(FILE*d,UJ n);
#else
#include <sys/types.h>
#include <unistd.h>
extern J zseek(FILE*d,J j,I f);
extern I ftrunc(FILE*d,UJ n);
#endif


extern UJ fsize(FILE *fp);

//:~
