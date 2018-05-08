typedef struct str { C s[0]; } SS;

C _cmp(V*,V*,size_t);
typedef C(*BIN_CMP_FN)(V*,V*,size_t);

#define NONE 0xffffffffffffffff	//< returned on empty result

//! compare SS,G,H,I,J
#define cmp(a,b,t) _cmp((t*)a, (t*)b, SZ(t))

//! perform binary search using cmp()
//! \param h haystack \param \n needle \param t type \param l items
//! \return (UJ)pos or NONE if nothing found
#define bin(h,n,t,l) _bin((t*)h, (t*)n, SZ(t), l)

//! perform binary search using custom comparator that returns (C)(-1;0;1)
//! \param h haystack \param \n needle \param t type \param l items \param fn
//! \return (UJ)pos or NONE if nothing found
#define binfn(h,n,t,l,fn) _binfn((t*)h, (t*)n, SZ(t), l, (BIN_CMP_FN)fn)

extern UJ _binfn(V*haystack, V*needle, size_t t, size_t len, BIN_CMP_FN cmpfn);

//:~