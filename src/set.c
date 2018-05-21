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
	s->items = vec_init_(1,el_size);
	s->cmpfn = cmpfn;
	R s;
}

V* set_get(SET s, V*key) {
	LOG("set_get");
	UJ i = binx_(s->items->data, key, s->items->el_size, s->items->used, (CMP)s->cmpfn);
	P(i==NIL, NULL);
	R vec_at_(s->items, i);
}

V* set_add(SET s, V*key) {
	LOG("set_add");

	//V*existing = set_get(s, key);
	//if(existing){T(TEST, "key is already in the set");}
	//P(existing, existing);

	if(s->items->used>0) {
		C res = s->cmpfn(key, vec_last_(s->items), s->items->el_size);
		P(res<=0, NULL)
		//X(res<=0, T(WARN, "inserting an unordered item is not supported"), NULL);
		//T(TEST, "comparison result=%d", res);
	}

	vec_add_((V**)&s->items, key);
	//R vec_last_(s->items);
	R0;
}

UJ set_size(SET s) {
	R s->items->used;
}

V set_destroy(SET s) {
	vec_destroy(s->items);
	free(s);
}

I set_intersection(SET a, SET b, SET dest) {
	C r = set_size(a)<set_size(b);
	SET needles = r?a:b; SET haystack = r?b:a;
	DO(set_size(needles),
		V*ndl = vec_at_(needles->items, i);
		if(set_get(haystack, ndl))
			set_add(dest, ndl);
	)
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
		//X(!obj, T(FATAL, "can't add to set"), 1);
		T(TEST, "added %ld", keys[i]);
	)

	//! dump vector
	//DO(vec_size(s->items), T(TEST, "%ld ", *(J*)vec_at_(s->items,i)))

	DO(7,
		J v = keys[i];
		J res = *(J*)set_get(s, &v);
		T(TEST, "got %ld", res);
	)

	J v1 = 9, v2 = 8, v3 = 2, v4 = 11;
	set_add(s, &v1); // ok
	set_add(s, &v2); // ok - already there
	set_add(s, &v3); // fail - out of order
	//TEND();

	SET s1 = set_init(SZ(J), (CMP)cmp_);
	//set_add(s1, &keys[0]);
	set_add(s1, &v3); // fail - out of order
	set_add(s1, &keys[1]);
	set_add(s1, &keys[2]);
	set_add(s1, &v1);
	set_add(s1, &v2);
	set_add(s1, &v3);
	set_add(s1, &v4);

	SET interx = set_init(SZ(J), (CMP)cmp_);
	set_intersection(s,s1,interx);

	TSTART();T(TEST,"(");
		DO(set_size(s),T(TEST, "%2ld ", *vec_at(s->items,i,J)));
		T(TEST, ") = %lu", set_size(s));
	TEND();

	TSTART();T(TEST,"(");
		DO(set_size(s1),T(TEST, "%2ld ", *vec_at(s1->items,i,J)));
		T(TEST, ") = %lu", set_size(s1));
	TEND();

	TSTART();T(TEST,"(");
		DO(set_size(interx),T(TEST, "%2ld ", *vec_at(interx->items,i,J)));
		T(TEST, ") = %lu", set_size(interx));
	TEND();


	T(TEST, "intersection len=%lu", set_size(interx));

	set_destroy(s);
	set_destroy(s1);
	set_destroy(interx);
	R0;
}

#endif


//:~