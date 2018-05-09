typedef struct string {C s[0];} SS;

C _cmp(V*,V*,sz);
typedef C(*BIN_CMP_FN)(V*,V*,sz);

//! compare SS,G,H,I,J
#define cmp(a,b,t) cmp_((t*)a, (t*)b, SZ(t))

//! perform binary search using cmp()
//! \param h haystack \param \n needle \param t type \param l items
//! \return (UJ)pos or NONE if nothing found
#define bin(h,n,t,l) bin_((t*)h, (t*)n, SZ(t), l)

//! perform binary search using custom comparator that returns (C)(-1;0;1)
//! \param h haystack \param \n needle \param t type \param l items \param fn
//! \return (UJ)pos or NONE if nothing found
#define binx(h,n,t,l,fn) binx_((t*)h, (t*)n, SZ(t), l, (BIN_CMP_FN)fn)

//! underlying implementation
extern UJ binx_(V*hst, V*ndl, sz t, sz len, BIN_CMP_FN cmpfn);
extern UJ bin_(V*hst, V*ndl, sz t, sz len);
extern  C cmp_(V*a, V*b, sz t);

//:~