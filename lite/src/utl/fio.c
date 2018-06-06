//! \file fio.c \brief file utilities

#include "../___.h"
#include "fio.h"

#if WIN32||_WIN64
J zseek(I d,J j,I f){UI h=(UI)(j>>32),l=SetFilePointer((HANDLE)d,(UI)j,&h,f);R((J)h<<32)+l;}
I ftrunc(FILE* d,UJ n){R zseek((I)d,n,0),SetEndOfFile((HANDLE)d)-1;}
#else
#include <sys/types.h>
#include <unistd.h>
J zseek(FILE* d,J j,I f){R fseek(d,j,f);}
I ftrunc(FILE* d,UJ n){R zseek(d,n,0),ftruncate(fileno(d),n);}
#endif

V* xmmap(S fname) {
	LOG("xmmap");
	FILE*f;xfopen(f, fname, "r", 0);
	sz fsz = fsize(f);
	fclose(f);
	I fd = open(fname, O_RDONLY, 0);
	V*r = mmap(0, fsz, PROT_READ, MAP_SHARED, fd, 0);
	X(r==MAP_FAILED, (close(fd),T(WARN, "mmap failed")), 0);
	close(fd); // safe to close now
	R r;}

V xmunmap(V*map,sz size){
	munmap(map, size);}

//! filesize
UJ fsize(FILE* fp) {
	LOG("fsize");
	X(!fp, T(WARN, "got empty fd"), NIL);
	UJ prev = ftell(fp);
	zseek(fp, 0L, SEEK_END);
	UJ fsz = ftell(fp);
	zseek(fp, prev, SEEK_SET);
	R fsz;
}

//! file exists
C fexist(S fpath){
	FILE* f;
	if((f=fopen(fpath,"r"))!=NULL)
		R!fclose(f); //< returns zero if closed
	R0; //< no file
}

#ifdef RUN_TESTS_FIO

ZI fio_test(){
	LOG("fio_test");
	FILE* fp;
	S f = "/etc/passwd";
	S f2 = "/tmp/nosuchfile";

	if(fexist(f))
		T(TEST, "OK -> %s exists", f);

	if(!fexist(f2))
		T(TEST, "OK -> %s does not exist", f2);
	
	xfopen(fp,f,"r",1);
	UJ file_size = fsize(fp);
	P(file_size==NIL, T(FATAL, "fsize(%s) failed", f)) //< returns 1
	T(TEST, "OK -> %s fsize=%lu", f, file_size);

	R0;
}

I main(){R fio_test();}

#endif


//:~