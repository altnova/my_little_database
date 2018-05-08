typedef struct str { C s[0]; } SS;

C _cmp(V*,V*,size_t);
typedef C(*BIN_CMP_FN)(V*,V*,size_t);

#define NONE 0xffffffffffffffff	//< returned on empty result

#define cmp(a,b,t) _cmp((t*)a, (t*)b, SZ(t))
#define bin(h,n,t,l) _bin((t*)h, (t*)n, SZ(t), l)
#define binfn(h,n,t,l,fn) _binfn((t*)h, (t*)n, SZ(t), l, (BIN_CMP_FN)fn)

extern UJ _binfn(V*haystack, V*needle, size_t t, size_t len, BIN_CMP_FN cmpfn);

//:~