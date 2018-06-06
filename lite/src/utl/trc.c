//!\file trc.c \brief logging system

#if WIN32||_WIN64
#else
#include <execinfo.h>
#endif 

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../___.h"
#include "../adt/bag.h"

ZC cont = 0;
ZC newline = 1;
ZC use_buf = 0;
ZC tmpbuf[1000];
Z BAG logbuf;

//! trace
I T(I lvl, const S fn, const S file, const I line, const S fmt, ...) {
	if(lvl<=LOGLEVEL) {
		va_list args;
		va_start(args, fmt);
		C buf[strlen(fn)+strlen(fmt)+strlen(file)+100];
		if (!cont) {
			OUT:snprintf(buf, SZ(buf), "  %s%s\e[0m %s:%d\t[%s] %s%c",
				lvl<L_INFO?"\e[1;33m":"\e[37m", loglevel_names[lvl], file, line, fn, fmt, newline?'\n':'\0');
			if(cont)newline = 0;
		} else {
			if(newline){newline=0;goto OUT;}
			snprintf(buf, SZ(buf), "%s", fmt);
		}
		if(use_buf) {
			sz len = vsnprintf(tmpbuf, 1000, buf, args);
			bag_add(logbuf,tmpbuf,len);
		}else 
			vprintf(buf, args);
		va_end(args);
	}
	R1; //< err
}

V TBUF(C sw) {
	if(sw) {
		if(!logbuf){
			logbuf = bag_init(1024);
		}
	} else {
		bag_destroy(logbuf);
	}
	use_buf=sw;
}

//! line continuation flag
V TSTART() {cont=1;}
V TEND() {O("\n");cont=0;newline=1;}

#if WIN32||_WIN64
V _stack(S msg, I d, I offset) {}
#else
V _stack(S msg, I d, I offset) {
 LOG("stack");
 V* array[d];
 sz size, i;
 S*strings;
 size = backtrace(array, d);
 strings = backtrace_symbols(array,size);
 DO(size-offset,O("%s %s\n",msg, strings[i+offset]));
 free(strings);}
#endif

V BYTES_AS_STR(S str,I n){DO(n,O("%c", (C)str[i]))}

//! print bits
V bits_char(C x, S dest) {
    C pos=0;
    for(I z=128; z>0; z>>=1)
    	dest[pos++]="01"[(x&z)==z];
    dest[pos]=0;
}

/*
//	T(TEST, "abo -> %d", !!tri_get(t, "abo"));
//	T(TEST, "abbot -> %d", !!tri_get(t, "abbot"));
//	T(TEST, "abolition -> %d", !!tri_get(t, "abbot"));

	tri_destroy(t);
	exit(0);

	C bits[9];
	O("%lu", SZ(long double));
	exit(0);
	DO(256,
		bits_char(i, bits);
		O("%lu b=%s\n", i, bits)
	)
*/

//:~