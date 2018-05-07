#include <stdlib.h>
#include <string.h>
#include "books.h"
#include "dynarray.h"

typedef J T;	//< array type in main()

Arr* _arr_init(size_t initialSize, size_t elSize) {
	Arr *a = (Arr*)malloc(SZ(Arr) + initialSize * elSize);
	a->used = 0;
	a->size = initialSize;
	a->el_size = elSize;
	R a;
}

Arr* _arr_add(Arr **aptr, V *el) {
	Arr *a = *aptr;
	if(a->used == a->size){
		a->size *= 2;
		*aptr = a = (Arr*)realloc(a, SZ(Arr) + a->size * a->el_size);
	}
	memcpy((V*)(a->data + a->el_size * a->used++), el, a->el_size);
	R *aptr;
}

V* _arr_at(Arr*a, J idx) {
	R (V*)(a->data + a->el_size * idx);
}

V arr_free(Arr *a) {
	free(a);
}

I main() {
	J i;
	I t = 100; //< test iterations

	Arr *a = arr_init(5, T);	//< initially 5 elements
	DO(t,arr_add(a, i)) //< will grow as necessary
	DO(t, O("%lu %lu|", i, arr_at(a, i, T)))
	O("\ntotal: %lu\n", a->used); //< print number of elements
	arr_free(a); //< don't forget!

	R 0;
}

//:~