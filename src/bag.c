//! \ bag.c \brief bag

#include <stdlib.h>
#include <string.h>
#include "___.h"
#include "cfg.h"
#include "trc.h"
#include "bag.h"

ZV zero(G*a,sz len){DO(len, a[i]=0)}

BAG bag_init(sz init_sz) {
	LOG("bag_init");
	X(init_sz<1,T(WARN, "initial size must be positive"), NULL);
	BAG h = (BAG)calloc(SZ_BAG,1);chk(h,NULL);
	h->ptr = calloc(1,init_sz);chk(h->ptr,NULL);
	h->size = init_sz;
	h->used = 0;
	R h;}

V* bag_add(BAG h, V*obj, sz obj_sz) {
	LOG("bag_add");
	sz offset = 0;
	if((obj_sz+h->used) > h->size){
		V*old_ptr = h->ptr;
		REALLOC:
		h->ptr = realloc(h->ptr, BAG_GROW_FACTOR * h->size);chk(h->ptr,NULL);
		zero(h->ptr+(h->size), h->size); //< zero out
		h->size *= BAG_GROW_FACTOR;
		if((obj_sz+h->used) > h->size)goto REALLOC;
		offset = old_ptr-(h->ptr);
		T(TEST, "realloc bag %lu, diff=%lu", h->size, offset);
	}
	V*obj_addr=h->ptr+(h->used);
	mcpy(obj_addr, obj, obj_sz);
	h->used += obj_sz;
	h->offset = offset;
	h->cnt++;
	R obj_addr;}

V bag_destroy(BAG h) {
	free(h->ptr);
	free(h);
}

#ifdef RUN_TESTS_BAG

I main() {
	LOG("bag_test");

	S keys[] = {"abbot", "abbey", "abacus", "abolition", "abolitions", "abortion", "abort", "zero"};

	BAG h=bag_init(1);
	X(!h,T(FATAL,"cannot init bag"),1);

	S obj;
	DO(8,
		obj = (S)bag_add(h, keys[i], scnt(keys[i]));
		X(!obj, T(FATAL, "can't add to bag"), 1);
		T(TEST, "added %s, offset=%ld", keys[i], h->offset))

	T(TEST,"added %lu objects", h->cnt);
	T(TEST,"bag contents: (%s)", h->ptr);

	bag_destroy(h);

	R 0;
}

#endif