//!\file arr.h \brief dynarray api

typedef struct dynarray {
	UJ hdr;			//< reserved
	sz used;		//< occupied
	sz size;		//< current capacity
	sz el_size;		//< element size
	G data[0];		//< struct hack
} pArr;

typedef pArr* Arr;
#define SZ_HDR SZ(pArr) //< array header size

//! public api
//! a array, el element, t type, n elements, i index
#define  arr_init(n,t)		({arr_init_(n,SZ(t));})
#define  arr_add(a,el)		({typeof(el)_e=el;arr_add_((V**)(&a),&_e);})
#define  arr_at(a,i,t)		({(t*)arr_at_(a,i);})
#define  arr_last(a,t)		({(t*)arr_last_(a);})
ext sz   arr_size(Arr a);
ext V    arr_destroy(Arr);		//< never forget

//! underlying implementation
ext Arr  arr_init_(sz n, sz t);
ext I    arr_add_(V**a, V*el);
ext V*   arr_at_(Arr a, UJ i);
ext V*   arr_last_(Arr a);

//:~