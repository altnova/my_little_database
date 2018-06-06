//!\file vec.h \brief vector api

#pragma once

typedef struct vector {
	sz size;		//< current capacity
	sz used;		//< occupied
	UI el_size;		//< element size
	 G data[0];		//< struct hack
} __attribute__((packed)) pVEC;

typedef pVEC* VEC;
#define SZ_VEC SZ(pVEC) //< vector header size

//! public api
//! \param a,el,t,n,i vector, element, type, count, index
#define  vec_init(n,t)		({vec_init_(n,SZ(t));})
#define  vec_add(a,el)		({typeof(el)_e=el;vec_add_((V**)(&a),&_e);})
#define  vec_at(a,i,t)		({(t*)vec_at_(a,i);})
#define  vec_last(a,t)		({(t*)vec_last_(a);})

ext sz vec_del_at(VEC a, sz i, sz n);  //! \param i start position, \param n how many
ext sz vec_clear(VEC a); //! truncate array
ext sz vec_size(VEC a);
ext sz vec_mem(VEC a);
ext E  vec_lfactor(VEC a);
ext V* vec_random(VEC a);
ext sz vec_compact(VEC*aptr);
ext C  vec_full(VEC a);
ext sz vec_destroy(VEC a); //< never forget

//! underlying implementation
ext VEC  vec_init_(sz n, UI t);
ext VEC  vec_add_(V**a, V*el);
ext V*   vec_at_(VEC a, UJ i);
ext V*   vec_last_(VEC a);

//:~