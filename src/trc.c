//!\file trc.c \brief logging system

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "___.h"
#include "cfg.h"
#include "trc.h"

ZC cont = 0;
ZC newline = 1;

//! trace
I T(I lvl, const S fn, const S file, const I line, const S fmt, ...) {
	if(lvl<=LOGLEVEL) {
		va_list args;
		va_start(args, fmt);
		C buf[strlen(fn)+strlen(fmt)+strlen(file)+30];
		if (!cont) {
			OUT:snprintf(buf, SZ(buf), "%s %s:%d\t[%s] %s%c", loglevel_names[lvl], file, line, fn, fmt, newline?'\n':'\0');
			if(cont)newline = 0;
		} else {
			if(newline){newline=0;goto OUT;}
			snprintf(buf, SZ(buf), "%s", fmt);
		}
		vprintf(buf, args);
		va_end(args);
	}
	R1; //< err
}

//! line continuation flag
V TSTART() {cont=1;}
V TEND() {O("\n");cont=0;newline=1;}


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