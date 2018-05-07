typedef struct str { C s[0]; } SS;

#define cmp(a,b,t) _cmp((t*)a, (t*)b, SZ(t))
#define bin(h,n,t,l) _bin((t*)h, (t*)n, SZ(t), l)
