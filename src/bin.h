//!\file bin.h \brief binary search api

typedef C(*CMP)(V*,V*,sz); //< comparator interface \return (-1;0;1)

//! compare SS,G,H,I,J
typedef struct string {C s[0];} SS; //< dummy type for passing strings to cmp()
#define cmp(a,b,t) cmp_((t*)a, (t*)b, SZ(t))

//! perform binary search using cmp()
//! \param h haystack \param \n needle \param t type \param l items
//! \return (UJ)pos or NIL if nothing found
#define bin(h,n,t,l) bin_((t*)h, (t*)n, SZ(t), l)

//! perform binary search using custom comparator 
//! \param h haystack \param \n needle \param t type \param l items \param fn
//! \return (UJ)pos or NIL if nothing found
#define binx(h,n,t,l,f) binx_((t*)h, (t*)n, SZ(t), l, (CMP)f)

//! underlying implementation
extern UJ binx_(V*hst, V*ndl, sz t, sz len, CMP cmpfn);
extern UJ bin_(V*hst, V*ndl, sz t, sz len);
extern  C cmp_(V*a, V*b, sz t);

//:~