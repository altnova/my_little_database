//!\file arr.c \brief dynarray implemenation

#include <stdlib.h>
#include <string.h>
#include "___.h"
#include "trc.h"
#include "arr.h"

Arr arr_init_(sz n, sz t) {
	LOG("arr_init");
	Arr a = (Arr)malloc(SZ_HDR + n * t);
	chk(a,0);
	a->used = 0;
	a->size = n;
	a->el_size = t;
	R a;}

V* arr_at_(Arr a, UJ i){
	P(i>a->used, NULL);
	R(V*)(a->data + i * a->el_size);}

V* arr_last_(Arr a){
	R(V*)(a->data + a->el_size * (a->used-1));}

V arr_destroy(Arr a){
	if(a)free(a);
	a = NULL;
}

sz arr_size(Arr a){
	R a->used;
}

ZC arr_full(Arr a){
	R a->used==a->size;
}

//! \return 0 -> ok, 1 -> err
I arr_add_(V** aptr, V* el){
	LOG("arr_add");
	Arr a = *aptr;
	if(arr_full(a)){
		a->size *= 2;
		a = realloc(a, SZ_HDR + a->el_size * a->size);chk(a,1);
		*aptr = a;
		T(TEST, "realloc to %p", *aptr);
		T(TEST,"grew to %lu", a->size);
	}
	memcpy((V*)(a->data + a->el_size * a->used++), el, a->el_size);
	R0;
}

//! test
typedef UJ TT; //< type
ZI arr_test(){
	LOG("arr_test");
	UJ t = 100; //< test iterations
	Arr a = arr_init(1,TT);	//< initially 10 elements
	X(!a,T(FATAL, "arr_init failed"),1)
	DO(t,O("%c", ".*"[arr_add(a,i)] )) //< will grow as necessary
	TSTART();
	DO(t,T(TEST, "%ld->%ld ", i, *arr_at(a,i,TT)))
	TEND();
	T(TEST,"capacity=%lu, used=%lu", a->size, arr_size(a)); //< print number of elements
	arr_destroy(a); //< never forget
	R0;
}

#ifdef RUN_TESTS_ARR
I main(){R arr_test();}
#endif


//:~

