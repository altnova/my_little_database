#define arr_init(initSize,type)({_arr_init(initSize,SZ(type));})
#define arr_add(a,val)({typeof(val) VAL = val; _arr_add(&a,&VAL);})
#define arr_at(a,idx,type)({*(type*)_arr_at(a,idx);})

typedef struct {
	size_t used;
	size_t size;
	size_t el_size;	
	G data[0];
} Arr;

extern Arr* _arr_init(size_t,size_t);
extern V _arr_add(Arr**, V*), arr_free(Arr*);
