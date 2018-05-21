//! \file set.c \brief ordered set

#include <stdlib.h>
#include <string.h>

#include "___.h"
#include "cfg.h"
#include "trc.h"
#include "bin.h"
#include "vec.h"
#include "set.h"

SET set_init(sz el_size, CMP cmpfn) {
	LOG("set_init");
	SET s = (SET)calloc(SZ_SET,1);chk(s,NULL);
	s->items = vec_init_(10,el_size);
	s->cmpfn = cmpfn;
	R s;
}

V*set_get(SET s, V*key) {
	LOG("set_get");
	UJ i = binx_(s->items->data, key, s->items->el_size, s->items->used, (CMP)s->cmpfn);
	P(i==NIL, NULL);
	R vec_at_(s->items, i);
}

V*set_add(SET s, V*key) {
	LOG("set_add");

	V*existing = set_get(s, key);
	//if(existing){T(TEST, "key is already in the set");}
	P(existing, existing);

	if(s->items->used>0) {
		C res = s->cmpfn(key, vec_last_(s->items), s->items->el_size);
		X(res<0, T(WARN, "inserting an unordered item is not supported"), NULL);
		//T(TEST, "comparison result=%d", res);
	}

	vec_add_(&s->items, key);
	R vec_last_(s->items);;
}

V set_destroy(SET s) {
	vec_destroy(s->items);
	free(s);
}

#ifdef RUN_TESTS_SET

I main() {
	LOG("set_test");

	J keys[] = {1,3,4,5,6,7,8};
	//S payloads[] = {"ABBOT", "ABBEY", "ABACUS", "ABOLITION", "ABOLITIONS", "ABORTION", "ABORT", "ZERO"};

	SET s=set_init(SZ(J), (CMP)cmp_);
	X(!s,T(FATAL,"cannot init set"),1);

	S obj;
	//TSTART();
	DO(7,
		obj = set_add(s, &keys[i]);
		X(!obj, T(FATAL, "can't add to set"), 1);
		T(TEST, "added %ld", keys[i]);
	)

	//! dump vector
	//DO(vec_size(s->items), T(TEST, "%ld ", *(J*)vec_at_(s->items,i)))

	DO(7,
		J v = keys[i];
		J res = *(J*)set_get(s, &v);
		T(TEST, "got %ld", res);
	)

	J v1 = 9, v2 = 8, v3 = 2;
	set_add(s, &v1); // ok
	set_add(s, &v2); // ok - already there
	set_add(s, &v3); // fail - out of order
	//TEND();

	set_destroy(s);
	R0;
}

#endif


//:~