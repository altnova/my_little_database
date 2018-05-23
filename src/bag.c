//! \file bag.c \brief bag

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
		//T(TEST, "realloc bag %lu, diff=%lu", h->size, offset);
	}
	V*obj_addr=h->ptr+(h->used);
	mcpy(obj_addr, obj, obj_sz);
	h->used += obj_sz;
	h->offset = offset;
	h->cnt++;
	R obj_addr;}

sz bag_mem(BAG h) {
	R SZ_BAG+h->size;}

E bag_lfactor(BAG h) {
	R (E)h->used/h->size;
}

sz bag_destroy(BAG h) {
	sz released = h->size;
	free(h->ptr);
	free(h);
	R SZ_BAG+released;
}

#ifdef RUN_TESTS_BAG

I main() {
	LOG("bag_test");

	S keys[] = {"abbot", "abbey", "abacus", "abolition", "abolitions", "abortion", "abort", "zero", "ninth"};
	S raw = "abbotabbeyabacusabolitionabolitionsabortionabortzeroninth";
	sz sz_raw = scnt(raw);

	BAG h=bag_init(1);

	ASSERT(h!=NULL, "should be able to create a bag")

	S obj; sz added = 0;
	DO(9,
		sz l = scnt(keys[i]);
		added += l;
		obj = (S)bag_add(h, keys[i], l))

	ASSERT(h->cnt==9, "bag_add() should work as expected (#1)")
	ASSERT(added==h->used, "bag_add() should work as expected (#2)")
	ASSERT(64==h->size, "bag should grow as expected")
	ASSERT(h->used==sz_raw, "bag should correctly report content size")
	ASSERT(!mcmp(h->ptr, raw, sz_raw), "bag content should match prediction")

	sz bytes = bag_mem(h);
	ASSERT(bytes==(SZ_BAG+64), "bag_mem() should report true size")
	ASSERT(IN(0.7, bag_lfactor(h), 0.9), "bag_lfactor() should report sane values")
	ASSERT(bytes==bag_destroy(h), "bag_destroy() should return bytes released")

	R0;
}

#endif

//:~
