#define arr_init(initSize,type)({_arr_init(initSize,SZ(type));})
#define arr_add(a,val)({typeof(val) VAL = val; _arr_add(&a,&VAL);})
#define arr_at(a,idx,type)({*(type*)_arr_at(a,idx);})
#define arr_last(a,type)({*(type*)_arr_last(a);})

typedef struct {
	UJ hdr;				//< placeholder for custom metadata
	size_t used;		//< occupied
	size_t size;		//< allocated
	size_t el_size;		//< element size
	G data[0];			//< array data
} Arr;

extern Arr* _arr_init(size_t initialSize, size_t elementSize);
extern V _arr_add(Arr**, V*);
extern V* _arr_at(Arr*a, J idx);
extern V* _arr_last(Arr*a);

extern V arr_free(Arr*);	//< don't forget!
