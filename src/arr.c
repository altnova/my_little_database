#include <stdlib.h>
#include <string.h>
#include "___.h"
#include "trc.h"
#include "arr.h"

Arr arr_init_(sz n, sz t) {
	Arr a = (Arr)malloc(SZ_HDR + n*t);
	a->used = 0;
	a->size = n;
	a->el_size = t;
	R a;}

V* arr_at_(Arr a, UJ i){
	P(i>a->used, NULL);
	R(V*)(a->data + i*a->el_size);}

V* arr_last_(Arr a){
	R(V*)(a->data + a->el_size*(a->used-1));}

V arr_free(Arr a){
	if(a)free(a),a=NULL;}

UJ arr_sz(Arr a){
	R a->used;}

Z C arr_full(Arr a){
	R arr_sz(a) == a->size;}

V arr_add_(Arr* aptr, V *el){
	LOG("arr_add");
	Arr a;
	if(arr_full(a=*aptr))
		a->size *= 2, T(TRACE,"grew to %lu", arr_sz(a)),
		*aptr = a = (Arr)realloc(a, SZ_HDR + a->el_size*a->size);
	memcpy((V*)(a->data + a->el_size*a->used++), el, a->el_size);}

//! test
typedef J TT; //< type
ZI arr_test(){
	UJ t = 100; //< test iterations
	Arr a = arr_init(5,TT);	//< initially 5 elements
	DO(t,arr_add(a,i)) //< will grow as necessary
	DO(t,O("%ld %ld|", i,*arr_at(a,i,TT)))
	O("\ntotal: %lu\n", arr_sz(a)); //< print number of elements
	arr_free(a); //< never forget
	R 0;}

#ifdef RUN_TESTS
I main(){R arr_test();}
#endif


//:~

