//!\file vec.c \brief vector implemenation

#include <stdlib.h>
#include <string.h>
#include "../___.h"
#include "vec.h"

VEC vec_init_(sz n, UI t) {
	LOG("arr_init");
	X(!t, T(WARN, "zero element width"), NULL);
	sz init_size = SZ_VEC + n * t;
	VEC a = (VEC)malloc(init_size); chk(a,0);
	a->used = 0;
	a->size = n;
	a->el_size = t;
	//a->grow_factor = 2;
	T(TRACE, "at %p", a);
	R a;}

V* vec_at_(VEC a, UJ i){
	P(i>=a->used, NULL);
	R(V*)(a->data + i * a->el_size);}

V* vec_random(VEC a) {
	R vec_at_(a, (UJ)rand()%a->used);
}

V* vec_last_(VEC a){
	R(V*)(a->data + a->el_size * (a->used-1));}

sz vec_destroy(VEC a){
	LOG("vec_destroy");
	T(DEBUG, "destroy at %p", a);
	sz released = vec_mem(a);
	free(a);
	a = NULL;
	R released;
}

sz vec_mem(VEC a){
	R SZ_VEC + a->el_size * a->size;
}

sz vec_size(VEC a){
	R a->used;
}

C vec_full(VEC a){
	R a->used==a->size;
}

//! \return pointer to self on ok, NULL on error
VEC vec_add_(V** aptr, V* el){
	LOG("vec_add");
	VEC a = *aptr;
	if(vec_full(a)) {
		//X(a->grow_factor<2, T(FATAL,"grow_factor is less than 2"), NULL)
		a->size *= 2;//a->grow_factor;
		sz new_size = SZ_VEC + a->el_size * a->size;
		a = realloc(a, new_size);chk(a,NULL);
		*aptr = a;
		T(DEBUG, "realloc to %lu (%p)", a->size, *aptr);
	}
	T(TRACE, "copying %d bytes", a->el_size);
	mcpy((V*)(a->data + a->el_size * a->used++), (V*)el, a->el_size);
	R a;
}

sz  vec_compact(VEC*aptr) {
	LOG("vec_compact");
	VEC a = *aptr;
	P(a->size==a->used, 0) // good enough already
	sz mem = vec_mem(a);
	a->size = MAX(1,a->used); // ensure at least 1 element
	//T(TEST, "size: %lu, used: %lu", a->size, a->used);
	sz new_size = SZ_VEC + a->el_size * a->size;
	a = realloc(a, new_size);chk(a,0);
	sz save = mem - new_size;
	*aptr = a;
	//T(TRACE, "compacted %lu bytes", save);
	R save;
}

E vec_lfactor(VEC a) {
	R (E)vec_mem(a)/(a->used * a->el_size);
}

sz vec_del_at(VEC a, sz i, sz n){
	LOG("vec_del_at");
	P(!n||i>=a->used,0)
	sz old_size = a->used;
	C truncate=0;
	if((i+n)>=a->used)truncate=1;
	if(truncate){
		// truncate!
		a->used = i;
		T(DEBUG, "truncated %lu elements", old_size - a->used);
	} else {
		V* dst = a->data + (i * a->el_size);
		V* src = dst + (n * a->el_size);
		sz bytes = (a->used-n) * a->el_size;
		P(dst!=memmove(dst, src, bytes), 0);
		a->used -= n;
		T(DEBUG, "deleted %lu elements", n);
	}
	//T(TEST, "load factor %0.3f, alloc=%lu alloc/2=%lu usage=%lu", vec_lfactor(a), a->mem, a->mem>>1, a->used * a->el_size);
	R a->used;} //< new size

sz vec_clear(VEC a) {
	R vec_del_at(a, 0, vec_size(a));
}

//! test
typedef UJ TT; //< type
ZI vec_test(){
	LOG("vec_test");
	UJ t = 20; //< test iterations
	VEC a = vec_init(1,TT);	//< initially 10 elements
	X(!a,T(FATAL, "vec_init failed"),1)
	DO(t,vec_add(a,i)) //< will grow as necessary
	TSTART();
	DO(vec_size(a),T(TEST, "%2ld ", *vec_at(a,i,TT)))
	T(TEST, "= %lu", vec_size(a));
	TEND();
	T(TEST,"capacity=%lu, used=%lu", a->size, vec_size(a)); //< print number of elements


	T(TEST, "deleting 5 elements at idx=0:");
	vec_del_at(a,0,5);
	TSTART();
	DO(vec_size(a),T(TEST, "%2ld ", *vec_at(a,i,TT)))
	T(TEST, "=%lu", vec_size(a));
	TEND();

	T(TEST, "new size %lu", vec_size(a));

	sz old_sz = vec_size(a);
	T(TEST, "deleting 100 elements at idx=3:");
	vec_del_at(a,3,100);
	TSTART();
	DO(vec_size(a),T(TEST, "%2ld ", *vec_at(a,i,TT)))
	T(TEST, "= %lu", vec_size(a));
	TEND();

	T(TEST, "new size %lu", vec_size(a));

	vec_destroy(a); //< never forget

	R0;
}

#ifdef RUN_TESTS_VEC
I main(){R vec_test();}
#endif


//:~

