#include <stdlib.h>
#include <string.h>
#include "books.h"
#include "dynarray.h"

typedef J T;	//< array type in main()

V _arr_init(Arr *a, size_t initialSize, size_t elSize) {
	a->data = (V*)malloc(initialSize * elSize);
	a->used = 0;
	a->size = initialSize;
	a->el_size = elSize;
}

V _arr_add(Arr *a, V *el) {
	if(a->used == a->size){
		a->size *= 2;
		a->data = (V*)realloc(a->data, a->size * a->el_size);
	}
	memcpy(((V**)(a->data + (a->el_size * a->used++))), el, a->el_size);
}

V* _arr_at(Arr*a, UJ idx) {
	return ((V**)(a->data + (a->el_size * idx)));
}

V arr_free(Arr *a) {
	free(a->data);
	a->data = NULL;
	a->used = a->size = 0;
}

I main() {
	Arr a;
	J i;
	arr_init(&a, 5, T);	//< initially 5 elements
	DO(100, arr_add(&a,i)) //< will auto resize as necessary
	O("el[9]: %lu\n", arr_at(&a, 9, T)); //< print nth element
	DO(100, O("%lu %lu|", i, arr_at(&a, i, T)))
	O("total: %lu\n", a.used); //< print number of elements
	arr_free(&a); //< don't forget!
}

//:~