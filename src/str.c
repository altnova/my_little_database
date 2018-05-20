//! \file str.c \brief test string utilities
#include <stdlib.h>
#include <string.h>
#include "___.h"
#include "trc.h"

#ifdef RUN_TESTS_STR

#define FTI_TOKEN_DELIM " \\%$`^0123456789#@_?~;&/\\,!|+-.:()[]{}<>*=\"\t\n\r'"

I main() {
	LOG("str_test");
	S str = "?a b cd. e?";
	I l = scnt(str);

	O("(%s)=%d\n", str, l);

	// tokenize forwards
	stok(str, l, FTI_TOKEN_DELIM, 0,
		O("len=%2ld, pos=%2ld, tok=", tok_len, tok_pos);
		BYTES_AS_STR(tok, tok_len);
		O("\n");
	)
	O("=======================\n");

	// tokenize backwards
	stok(str, l, FTI_TOKEN_DELIM, 1,
		O("len=%2ld, pos=%2ld, tok=", tok_len, tok_pos);
		BYTES_AS_STR(tok, tok_len);
		O("\n");
	)
	O("\n");

}

#endif

//:~
