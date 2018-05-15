//! \ hea.c \brief heap

#include <stdlib.h>
#include <string.h>
#include "___.h"
#include "cfg.h"
#include "trc.h"
#include "hea.h"

ZV zero(G*a,sz len){DO(len, a[i]=0)}

HEAP hea_init(sz init_sz) {
	LOG("hea_init");
	X(init_sz<1,T(WARN, "initial size must be positive"), NULL);
	HEAP h = (HEAP)calloc(SZ_HEAP,1);chk(h,NULL);
	h->ptr = calloc(1,init_sz);chk(h->ptr,NULL);
	h->size = init_sz;
	R h;}

V* hea_add(HEAP h, V*obj, sz obj_sz) {
	LOG("hea_add");
	sz diff = 0;
	if(h->used+obj_sz > h->size){
		V*old_ptr = h->ptr;
		h->ptr = realloc(h->ptr, HEA_GROW_FACTOR * h->size);chk(h->ptr,NULL);
		zero(h->ptr+(h->size), h->size); //< zero out
		h->size *= HEA_GROW_FACTOR;
		diff = old_ptr-(h->ptr);
	}
	V*heap_addr=h->ptr+(h->used);
	mcpy(heap_addr, obj, obj_sz);
	h->used += obj_sz;
	h->diff = diff;
	h->cnt++;
	R heap_addr;}

V hea_destroy(HEAP h) {
	free(h->ptr);
	free(h);
}

#ifdef RUN_TESTS_HEA

I main() {
	LOG("hea_test");

	S keys[] = {"abbot", "abbey", "abacus", "abolition", "abolitions", "abortion", "abort", "zero"};

	HEAP h=hea_init(1024);
	X(!h,T(FATAL,"cannot init heap"),1);

	S obj;
	DO(8,
		obj = (S)hea_add(h, keys[i], scnt(keys[i]));
		X(!obj, T(FATAL, "can't add to heap"), 1);
		T(TEST, "added %s, diff=%ld", obj, h->diff))

	T(TEST,"added %lu objects", h->cnt);
	T(TEST,"heap contents: (%s)", h->ptr);

	hea_destroy(h);

	R 0;
}

#endif