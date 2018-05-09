#include <errno.h>
#include "___.h"
#include "trc.h"

#if WIN32||_WIN64
J zseek(I d,J j,I f){UI h=(UI)(j>>32),l=SetFilePointer((HANDLE)d,(UI)j,&h,f);R((J)h<<32)+l;}
I ftrunc(FILE*d,UJ n){R zseek((I)d,n,0),SetEndOfFile((HANDLE)d)-1;}
#else
#include <sys/types.h>
#include <unistd.h>
J zseek(FILE*d,J j,I f){R fseek(d,j,f);}
I ftrunc(FILE*d,UJ n){R zseek(d,n,0),ftruncate(fileno(d),n);}
#endif

//! filesize
UJ fsize(FILE *fp) {
	LOG("fsize");
	X(!fp, T(WARN, "got empty fd"), NIL);
	UJ prev = ftell(fp);
	zseek(fp, 0L, SEEK_END);
	UJ sz = ftell(fp);
	zseek(fp, prev, SEEK_SET);
	R sz;
}

//:~