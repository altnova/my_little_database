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

SET set_clone(SET s, SET from) {
	LOG("set_clone");
	s->cmpfn = from->cmpfn;
	sz vsize = vec_mem(from->items);
	s->items = (VEC)realloc(s->items, vsize);chk(s->items,NULL);
	mcpy(s->items,from->items,vsize);
	R s;
}

V* set_get(SET s, V*key) {
	LOG("set_get");
	UJ i = binx_(s->items->data, key, s->items->el_size, s->items->used, (CMP)s->cmpfn);
	P(i==NIL, NULL);
	R vec_at_(s->items, i);
}

C set_add(SET s, V*key) {
	LOG("set_add");

	//V*existing = set_get(s, key);
	//if(existing){T(TEST, "key is already in the set");}
	//P(existing, existing);

	if(s->items->used>0) {
		C res = s->cmpfn(key, vec_last_(s->items), s->items->el_size);
		P(res<=0, 0)
		//X(res<=0, T(WARN, "inserting an unordered item is not supported"), NULL);
		//T(TEST, "comparison result=%d", res);
	}

	vec_add_((V**)&s->items, key);
	//R vec_last_(s->items);
	R1;
}

sz set_add_all(SET s, V*keys, sz n) {
	I r = 0;
	DO(n, r += set_add(s, keys + i * s->items->el_size))
	R r;}

sz set_size(SET s) {
	R s->items->used;
}

sz set_mem(SET s) {
	R SZ_SET + vec_mem(s->items);
}

V set_clear(SET s) {
	vec_clear(s->items);}

V set_intersection(SET a, SET b, SET dest) {
	//C r = set_size(a)<=set_size(b);
	//SET needles = r?a:b; SET haystack = r?b:a;
	SET needles = a, haystack = b;
	DO(set_size(needles),
		V*ndl = vec_at_(needles->items, i);
		if(set_get(haystack, ndl))
			set_add(dest, ndl);
	)}

sz set_compact(SET s) {
	R vec_compact(&s->items);}

C set_contains(SET s, SET subset) {
	DO(set_size(subset),
		P(set_get(s, vec_at_(subset->items,i))==NULL,0)
	)
	R1;}

sz set_destroy(SET s) {
	sz dealloc = set_mem(s);
	sz rel = vec_destroy(s->items);
	free(s);
	R dealloc;}

#ifdef RUN_TESTS_SET

I main() {
	LOG("set_test");

	J keys[] = {1,3,4,5,6,7,8};

	SET s=set_init(SZ(J), (CMP)cmp_);

	ASSERT(s!=NULL, "should be able to create a set")

	I added = 0;
	DO(7, added += set_add(s, &keys[i]))
	ASSERT(set_size(s)==7&&added==7, "set_add() should work as expected")

	I retrieved = 0;
	DO(7,
		J v = keys[i];
		J res = *(J*)set_get(s, &v);
		retrieved += v==res;)
	ASSERT(retrieved==7, "set_get() should work as expected (#1)");

	J v1 = 9, v2 = 8, v3 = 2, v4 = 11, v5 = 13;
	ASSERT(set_get(s, &v5)==NULL, "set_get() should work as expected (#2)")
	ASSERT(set_add(s, &v1), "adding an ordered item should work")
	ASSERT(!set_add(s, &v2), "adding an exising item should have no effect")
	ASSERT(!set_add(s, &v3), "adding an unordered item should have no effect")

	J more_keys[] = {10,11,12};

	ASSERT(set_add_all(s,&more_keys,3)==3, "set_add_all() should work as expected")

	SET subset = set_init(SZ(J),(CMP)cmp_);
	set_add_all(subset,&more_keys,3);
	ASSERT(set_contains(s,subset)==1, "set_contains() should work as expected (#1)")

	set_add(subset, &v5);
	ASSERT(set_contains(s,subset)==0, "set_contains() should work as expected (#2)")

	SET s1 = set_init(SZ(J), (CMP)cmp_);
	set_add(s1, &v3);
	set_add(s1, &keys[1]);
	set_add(s1, &keys[2]);
	set_add(s1, &v1);
	set_add(s1, &v2);
	set_add(s1, &v3);
	set_add(s1, &v4);

	SET interx = set_init(SZ(J), (CMP)cmp_);
	set_intersection(s1,s,interx);

	J ex[] = {3,4,9,11};
	SET expected = set_init(SZ(J), (CMP)cmp_);
	set_add_all(expected,ex,4);

	ASSERT(set_size(interx)==4&&set_contains(interx,expected), "set_intersection() should work as expected")

	/*
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

	T(TEST, "intersection len=%lu, at=%p", set_size(interx), interx);
	*/

	SET cloned = set_init(SZ(J), (CMP)cmp_);
	set_clone(cloned, interx);
	ASSERT(set_size(cloned)==set_size(interx)&&cloned!=interx,
		"cloned set should have same size and different address");

	J one_more = 100;
	set_add(s, &one_more);

	sz mem1 = set_mem(s);
	//T(TEST, "before compact: %lu items, %lu bytes", set_size(s), mem1);
	sz save = set_compact(s);
	sz mem2 = set_mem(s);
	//T(TEST, "after  compact: %lu items, %lu bytes, save=%lu", set_size(s), mem2, save);

	ASSERT(mem2<mem1, "set_compact() should have effect")
	ASSERT(save>0, "set_compact() savings should be measurable")

	ASSERT(set_destroy(s)==mem2, "set_destroy() should return bytes released")
	set_destroy(s1);
	set_destroy(interx);
	set_destroy(cloned);
	R0;
}

#endif


//:~