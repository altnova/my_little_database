//! \file rnd.c \brief randomness

#include <stdlib.h>
#include <string.h>
#include "../___.h"
#include "rnd.h"

S rnd_str(S dest, sz size, I charset) {
	S dict;
	SW(charset){
		CS(0,dict="ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789")
		CS(1,dict="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ")
		CS(2,dict="ABCDEFGHIJKLMNOPQRSTUVWXYZ")
		CS(3,dict="abcdefghijklmnopqrstuvwxyz")
	}
	sz dictlen = scnt(dict);
	DO(size,
		sz key = rand()%dictlen;
		dest[i] = dict[key];
	)
	dest[size] = '\0'; //< terminate string
	R dest;
}

#ifdef RUN_TESTS_RND

I rnd_test() {
	LOG("rnd_test");
	sz l = 10;
	DO(1,
		S s = (S)malloc(l+1);chk(s,1);
		T(TEST, "CHARSET_ALNUM %s", rnd_str(s,l,CHARSET_ALNUM));
		T(TEST, "CHARSET_AZaz  %s", rnd_str(s,l,CHARSET_AZaz));
		T(TEST, "CHARSET_AZ    %s", rnd_str(s,l,CHARSET_AZ));
		T(TEST, "CHARSET_az    %s", rnd_str(s,l,CHARSET_az));
		free(s))
	R0;
}

I main() { R rnd_test(); }

#endif
