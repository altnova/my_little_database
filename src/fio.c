//!\file fio.c \brief file utilities

#include <stdlib.h>
#include <errno.h>
#include "___.h"
#include "trc.h"
#include "fio.h"

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

//! file exists

C fexist(S fpath){
	FILE *f;
    if((f=fopen(fpath,"r"))!=NULL){
    	R!fclose(f); //< returns zero if closed
    }
    R 0; //< no file
}

//! test
ZI fio_test(){
	LOG("fio_test");
	FILE*fp;
	S f = "/etc/passwd";
	S f2 = "/tmp/nosuchfile";

	if(fexist(f))
		T(TEST, "OK -> %s exists", f);

	if(!fexist(f2))
		T(TEST, "OK -> %s does not exist", f2);
	
	xfopen(fp,f,"r",1);
	UJ fsz = fsize(fp);
	P(fsz==NIL, T(FATAL, "fsize(%s) failed", f)) //< returns 1
	T(TEST, "OK -> %s fsize=%lu", f, fsz);

	R 0;}

#ifdef RUN_TESTS
I main(){R fio_test();}
#endif


//:~