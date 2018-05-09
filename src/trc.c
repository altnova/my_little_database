#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "___.h"
#include "cfg.h"
#include "trc.h"

//! trace
I T(I lvl, S fn, S file, I line, S fmt, ...) {
	if(lvl <= LOGLEVEL) {
		va_list args;
		va_start(args, fmt);
		C buf[strlen(fn)+strlen(fmt)+strlen(file)+30];
		snprintf(buf, SZ(buf), "%s:%d\t[%s] %s\n", file, line, fn, fmt);
		vprintf(buf, args);
		va_end(args);
	}
	R 1; //< err
}


//:~