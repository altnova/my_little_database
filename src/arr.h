
typedef struct dynarray {
	UJ hdr;			//< reserved
	sz used;		//< occupied
	sz size;		//< allocated
	sz el_size;		//< element size
	G data[0];		//< struct hack
} pArr;

typedef pArr* Arr;
#define SZ_HDR SZ(pArr) //< array header size

#define arr_init(n,t)({arr_init_(n,SZ(t));})
#define arr_add(a,val)({typeof(val)v=val;arr_add_(&a,&v);})
#define arr_at(a,i,t)({(t*)arr_at_(a,i);})
#define arr_last(a,t)({(t*)arr_last_(a);})

extern Arr  arr_init_(sz n, sz t);
ext V    arr_add_(Arr*a, V*el);
ext V*   arr_at_(Arr a, UJ i);
ext V*   arr_last_(Arr a);
ext UJ   arr_sz(Arr a);

ext V    arr_free(Arr);	//< never forget

//:~