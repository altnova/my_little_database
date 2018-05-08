#include <stdarg.h>
#include <stdio.h>
#include "books.h"
#include "trace.h"

//! trace
V T(I lvl, S format, ...) {
	va_list args;
	va_start(args, format);

	if(lvl <= DEBUG_LEVEL)
		vprintf(format, args);

	va_end(args);
}
