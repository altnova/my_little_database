//!\file vec.c \brief dynarray implemenation

#include <stdlib.h>
#include <string.h>
#include "___.h"
#include "trc.h"
#include "vec.h"

Vec vec_init_(sz n, sz t) {
	LOG("arr_init");
	sz init_size = SZ_HDR + n * t;
	Vec a = (Vec)malloc(init_size);
	chk(a,0);
	a->mem = init_size;
	a->used = 0;
	a->size = n;
	a->el_size = t;
	a->grow_factor = 2;
	T(TRACE, "at %p", a);
	R a;}

V* vec_at_(Vec a, UJ i){
	P(i>a->used, NULL);
	R(V*)(a->data + i * a->el_size);}

V* vec_last_(Vec a){
	R(V*)(a->data + a->el_size * (a->used-1));}

sz vec_destroy(Vec a){
	LOG("vec_destroy");
	T(TRACE, "destroy at %p", a);
	sz released = a->mem;
	if(a)free(a);
	a = NULL;
	R released;
}

sz vec_size(Vec a){
	R a->used;
}

ZC vec_full(Vec a){
	R a->used==a->size;
}

//! \return pointer to self on ok, NULL on error
Vec vec_add_(V** aptr, V* el){
	LOG("vec_add");
	Vec a = *aptr;
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

//! test
typedef UJ TT; //< type
ZI vec_test(){
	LOG("vec_test");
	UJ t = 100; //< test iterations
	Vec a = vec_init(1,TT);	//< initially 10 elements
	X(!a,T(FATAL, "vec_init failed"),1)
	DO(t,vec_add(a,i)) //< will grow as necessary
	TSTART();
	DO(t,T(TEST, "%ld->%ld ", i, *vec_at(a,i,TT)))
	TEND();
	T(TEST,"capacity=%lu, used=%lu", a->size, vec_size(a)); //< print number of elements
	vec_destroy(a); //< never forget
	R0;
}

#ifdef RUN_TESTS_VEC
I main(){R vec_test();}
#endif


//:~

