//!\file bin.c \brief binary search implementation

#include <stdlib.h>
#include <string.h>
#include "___.h"
#include "trc.h"
#include "bin.h"

#define val(x,t) *((t*)x) 
#define diff(t) val(x,t)-val(y,t)
#define DBG 	T(TRACE, "l=%lu m=%lu h=%lu needle=%d hay(%lu)=%d r=%d\n", \
					l, m, h, val(ndl,I), m, val(hst+m * t,I), r)
//! compare
C cmp_(V*x, V*y, sz t){				 //< sw/cs?
	J r =							 //< compare:
		t==SZ(SS) ?scmp(x,y):		 //< strs
		t==SZ(C)  ?diff(C): 		 //< chars
		t==SZ(H)  ?diff(H): 		 //< shorts
		t==SZ(I)  ?diff(I): 		 //< ints
		t==SZ(J)  ?diff(J): 		 //< longs
				   memcmp(x,y,t); //< everything
	R!r?r:r<0?-1:1;}		//< (lt,eq,gt)

//! \brief haystack, needle, type, length, comparator
UJ binx_(V*hst, V*ndl, sz t, sz n, CMP cfn){
	LOG("binx");P(!n,NIL);P(n==1,cfn(hst,ndl,t)?NIL:0);	//< fail fast
	P(cfn(hst,ndl,t)>0||cfn((hst+(n-1) * t),ndl,t)<0,NIL); //< check range
	UJ l=0,h=n,m;C r=1; //< lo,hi,mi,res
	W((l<h)&r)if(m=(l+h)/2,r=cfn((hst+m * t),ndl,t),r>=0)h=m;else l=m+1;
	R r?NIL:m;}

//! use native comparator
UJ bin_(V* hst,V* ndl,sz t,sz l){
	R binx_(hst,ndl,t,l,&cmp_);}

//! test
typedef J TT; //< type
Z TT hst[11] = { 0, 2, 8, 10, 14, 75, 100, 101, 120, 9999, 10000 }; //< haystack
Z TT edge_cases[4] = { -1, 1, 9998, 10001 }; //< negatives
Z TT ndl; //< needle

ZI bin_test() {
	LOG("bin_test");
	J a = 1, b = 2; UJ r;
	//! test cmp fn
	T(DEBUG, "S %d", cmp("abc", "abcd", SS));
	T(DEBUG, "C %d", cmp(&a, &b, C));
	T(DEBUG, "H %d", cmp(&a, &b, H));
	T(DEBUG, "I %d", cmp(&a, &b, I));
	T(DEBUG, "J %d", cmp(&a, &b, J));

	DO(11, //< hits
		ndl = hst[i];
		r=bin(&hst, &ndl, TT, 11);
		T(INFO, "%s   ->   bin[%ld]=%lu, expect %d", hst[r]==ndl?"OK":"FAIL", r, hst[r], ndl);
	)

	DO(1, //< degraded
		ndl = hst[i];
		r=bin(&hst, &ndl, TT, 1);
		T(INFO, "%s   ->   bin[%ld]=%lu, expect %d", hst[r]==ndl?"OK":"FAIL", r, hst[r], ndl);
	)

	DO(4, //< misses
		ndl = edge_cases[i];
		r = bin(&hst, &ndl, TT, 11);
		T(INFO, "%s   ->   bin[%ld]=%lu, expect %d", r==-1?"OK":"FAIL", r, hst[r], -1);
	)
	
	R0;
}

#ifdef RUN_TESTS
I main(){R bin_test();}
#endif

//:~