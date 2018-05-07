#include <stdlib.h>
#include <string.h>
#include "books.h"
#include "binsearch.h"

typedef I BINTYPE; //< type for test array

C _cmp(V*a, V*b, size_t t){
	J r;
	r=							 //< compare:
		t==0 ?scmp(a,b):		 //< strings
		t==1 ?*((G*)a)-*((G*)b): //< bytes and chars
		t==2 ?*((H*)a)-*((H*)b): //< shorts
		t==4 ?*((I*)a)-*((I*)b): //< ints
		t==8 ?*((J*)a)-*((J*)b): //< longs
			 memcmp(a,b,t);		 //< everything else
	R !r?r:r<0?-1:1;}			 //< -1,0,1 le,eq,ge 

UJ _binfn(V*haystack, V*needle, size_t t, size_t len, BIN_CMP_FN cmpfn){
 if(len<1)R -1;if(len==1)R cmpfn(haystack,needle,t)?-1:0;	//< len<2
 if(cmpfn(haystack,needle,t)>0||cmpfn(haystack+(len-1)*t,needle,t)<0)R -1; //< out of range
 UJ l=0,h=len,i; C r=1; //< lo, hi, mid
 for(;l<h&r;)if(i=(l+h)/2,r=cmpfn(haystack+i*t,needle,t),r>=0)h=i;else l=i+1;
 O("l=%lu i=%lu h=%lu needle=%d hay(%lu)=%d r=%d\n", l, i, h, *((I*)needle), i, *((I*)(haystack+i*t)), r); //< debug
 R r?-1:i;}

UJ _bin(V*haystack, V*needle, size_t t, size_t len){
	R _binfn(haystack, needle, t, len, (BIN_CMP_FN)&_cmp);
}

BINTYPE arr[11] = { 0, 2, 8, 10, 14, 75, 100, 101, 120, 9999, 10000 };
BINTYPE edge_cases[4] = { -1, 1, 9998, 10001 };
BINTYPE needle;

Z I bin_test() {
	J a = 1, b = 2; UJ r;

	//! test cmp function
	O("S %d\n", cmp("abc", "abcd", SS));
	O("C %d\n", cmp(&a, &b, C));
	O("H %d\n", cmp(&a, &b, H));
	O("I %d\n", cmp(&a, &b, I));
	O("J %d\n", cmp(&a, &b, J));

	DO(11, //< hits
		needle = arr[i];
		r=bin(&arr, &needle, BINTYPE, 11);
		O("bin[%lu] = %d, expect %d (%s)\n", r, arr[r], needle, arr[r]==needle?"OK":"FAIL");
	)

	DO(1, //< degraded
		needle = arr[i];
		r=bin(&arr, &needle, BINTYPE, 1);
		O("bin[%lu] = %d, expect %d (%s)\n", r, arr[r], needle, arr[r]==needle?"OK":"FAIL");
	)

	DO(4, //< misses
		needle = edge_cases[i];
		r=bin(&arr, &needle, BINTYPE, 11);
		O("bin[%d] = %lu, expect %d (%s)\n", needle, r, -1, r==-1?"OK":"FAIL");
	)

	R 0;
}

//I main() {R bin_test();}

//:~