#include <stdlib.h>
#include <string.h>
#include "_.h"
#include "arr.h"

typedef J TYPE;	//< array type in main()

Arr* _arr_init(size_t initialSize, size_t elSize) {
	Arr *a = (Arr*)malloc(SZ(Arr) + initialSize * elSize);
	a->used = 0;
	a->size = initialSize;
	a->el_size = elSize;
	R a;
}

V _arr_add(Arr **aptr, V *el) {
	Arr *a = *aptr;
	if(a->used == a->size){
		a->size *= 2;
		*aptr = a = (Arr*)realloc(a, SZ(Arr) + a->size * a->el_size);
	}
	memcpy((V*)(a->data + a->el_size * a->used++), el, a->el_size);
}

V* _arr_at(Arr*a, UJ idx) {
	if(idx>a->used)R NULL;
	R (V*)(a->data + a->el_size * idx);
}

V* _arr_last(Arr*a) {
	R (V*)(a->data + a->el_size * (a->used-1));
}

V arr_free(Arr *a) {
	if (a)free(a);
}

Z I arr_test() {
	J i;
	I t = 100; //< test iterations

	Arr *a = arr_init(5, TYPE);	//< initially 5 elements
	DO(t,arr_add(a, i)) //< will grow as necessary
	DO(t, O("%ld %ld|", i, *arr_at(a, i, TYPE)))
	O("\ntotal: %lu\n", a->used); //< print number of elements
	arr_free(a); //< don't forget!

	R 0;
}

//Z I main() {R arr_test();}

//:~