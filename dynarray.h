#define arr_init(a,initSize,type)({_arr_init(a,initSize,SZ(type));})
#define arr_add(a,val) {typeof(val) VAL = val; _arr_add(a,&VAL);}
#define arr_at(a,idx,type)({*(type*)_arr_at(a,idx);})

typedef struct {
	V*		 data;	
	size_t used;
	size_t size;
	size_t el_size;	
} Arr;
