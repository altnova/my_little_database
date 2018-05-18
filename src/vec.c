//!\file vec.c \brief vector implemenation

#include <stdlib.h>
#include <string.h>
#include "___.h"
#include "trc.h"
#include "vec.h"

VEC vec_init_(sz n, sz t) {
	LOG("arr_init");
	X(!t, T(WARN, "zero element width"), NULL);
	sz init_size = SZ_HDR + n * t;
	VEC a = (VEC)malloc(init_size); chk(a,0);
	a->mem = init_size;
	a->used = 0;
	a->size = n;
	a->el_size = t;
	a->grow_factor = 2;
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
	T(TRACE, "destroy at %p", a);
	sz released = a->mem;
	if(a)free(a);
	a = NULL;
	R released;
}

sz vec_mem(VEC a){
	R a->mem;
}

sz vec_size(VEC a){
	R a->used;
}

ZC vec_full(VEC a){
	R a->used==a->size;
}

//! \return pointer to self on ok, NULL on error
VEC vec_add_(V** aptr, V* el){
	LOG("vec_add");
	VEC a = *aptr;
	if(vec_full(a)) {
		X(a->grow_factor<2, T(FATAL,"grow_factor is less than 2"), NULL)
		a->size *= a->grow_factor;
		sz new_size = SZ_HDR + a->el_size * a->size;
		a = realloc(a, new_size);chk(a,NULL);
		a->mem = new_size;
		*aptr = a;
		T(TRACE, "realloc to %lu (%p)", a->size, *aptr);
	}
	memcpy((V*)(a->data + a->el_size * a->used++), el, a->el_size);
	R a;
}

E vec_lfactor(VEC a) {
	R a->mem/(a->used * a->el_size);
}

sz vec_del_at(VEC a, sz i, sz n){
	LOG("vec_del_at");
	P(!n||i>=a->used,0)
	sz old_size = a->used;
	C truncate=0;
	if((i+n+1)>=a->used)truncate=1;
	if(truncate){
		// truncate!
		a->used = i;
		T(TEST, "truncated %lu elements", old_size - a->used);
	} else {
		V* dst = a->data + (i * a->el_size);
		V* src = dst + (n * a->el_size);
		sz bytes = (a->used-n) * a->el_size;
		P(dst!=memmove(dst, src, bytes), 0);
		a->used -= n;
		T(TEST, "deleted %lu elements", n);
	}
	T(TEST, "load factor %0.3f, alloc=%lu alloc/2=%lu usage=%lu", vec_lfactor(a), a->mem, a->mem>>1, a->used * a->el_size);
	R a->used;} //< new size

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

