//!\file vec.h \brief dynamic Vecay api

typedef struct vector {
	UJ hdr;			//< reserved
	sz used;		//< occupied
	sz size;		//< current capacity
	sz mem;			//< memory utilization
	C grow_factor;	//< size multiplier on realloc, defaults to 2, never set to <2
	sz el_size;		//< element size
	G data[0];		//< struct hack
} pVec;

typedef pVec* Vec;
#define SZ_HDR SZ(pVec) //< vector header size

//! public api
//! \param a,el,t,n,i vector, element, type, elements, index
#define  vec_init(n,t)		({vec_init_(n,SZ(t));})
#define  vec_add(a,el)		({typeof(el)_e=el;vec_add_((V**)(&a),&_e);})
#define  vec_at(a,i,t)		({(t*)vec_at_(a,i);})
#define  vec_last(a,t)		({(t*)vec_last_(a);})
ext sz   vec_size(Vec a);
ext sz   vec_mem(Vec a);
ext sz   vec_destroy(Vec);		//< never forget

//! underlying implementation
ext Vec  vec_init_(sz n, sz t);
ext Vec  vec_add_(V**a, V*el);
ext V*   vec_at_(Vec a, UJ i);
ext V*   vec_last_(Vec a);

//:~