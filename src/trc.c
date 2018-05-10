//!\file trc.c \brief logging system

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "___.h"
#include "cfg.h"
#include "trc.h"

Z C cont = 0;
Z C newline = 1;

//! trace
I T(I lvl, S fn, S file, I line, S fmt, ...) {
	if(lvl <= LOGLEVEL) {
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
	R 1; //< err
}

//< line continuation flag
V TSTART() {cont=1;}
V TEND() {O("\n");cont=0;newline=1;}

//:~