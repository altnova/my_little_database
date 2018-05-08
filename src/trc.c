#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "___.h"
#include "cfg.h"
#include "trc.h"

//! trace
I T(I lvl, S fn, S file, I line, S format, ...) {
	va_list args;
	va_start(args, format);
	if(lvl <= LOGLEVEL) {
		C buf[strlen(fn)+strlen(format)+strlen(file)+30];
		snprintf(buf, sizeof buf, "%s:%d\t[%s] %s\n", file, line, fn, format);
		vprintf(buf, args);
	}
	va_end(args);
	R 1; //< err
}


//:~