#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "___.h"
#include "trc.h"

typedef struct bucket{I h,n;struct bucket *next;I usage;C s[];} *bkt;
Z sz SZ_BKT = SZ(struct bucket); //< struct header size
ZI split, level=2;
Z bkt* buckets;

//! https://groups.google.com/forum/#!topic/comp.lang.c/lSKWXiuNOAk
I djb(G*a,UI n){I h=5381;DO(n,h=33*(h^a[i]))R h;}

//! copy with seek \param d dest \param s source \param n len
S dsn(V*d,V*s,UJ n){R(S)memcpy(d,s,n)+n;}

Z UJ mem, cnt;		//< hashtable bytesize and element counters

I hsh_print(bkt b) {
	P(!b,NULL);
	//if(!b){O("empty\n");R;
	O("================================================\n");
	O("s    -> %s\n",   b->s);
	O("idx  -> %d\n",   b->usage);
	O("hsh  -> %d\n",   b->h);
	O("len  -> %d\n",   b->n);
	O("next -> %d\n",   b->next?(b->next)->usage:-1);
}

//! lookup str in the hash table \param s str
//! \see https://svn.process-one.net/ejabberd/tags/ejabberd-2.0.5/src/tls/tls_drv.c
//! \return ptr to permanent address
S hsh_get(S s){
	LOG("hsh_get");
	O("\n");
	I n = scnt(s);
	bkt B;
	I hi=1, rec_len = SZ_BKT+n+1, hash=djb(s,n), idx=hash&(level-1);

	T(WARN, "GET  ->   %s   ->   %d\n", s, idx);
	O("================================================\n");

	if(idx<split){
		hi = 0;
		O("********** b_id(%d)<split(%d) -> b=%d\n", idx, split, hash&((level<<1)-1));
		O("********** lvl=%d shifted=%d\n", level, level<<1);
		idx=hash&((level<<1)-1);
	}

	//! lookup the string
	B=buckets[idx];
	I depth = 0;
	while(B){								//< inspect the linked list from head
		if(B->n==n){						//< if length matches...
	 		DO(n,if(B->s[i]!=s[i])goto L0)	//< compare bytes, L0 on first mismatch
	 		T(WARN, "FOUND -> %s (depth=%d)", B->s, depth);
	 		hsh_print(B);
	 		R B->s;}						//< found a match, return the ptr				
		L0:depth++;B=B->next;}				//< move to next linked node
										
	O("%s %lu\tbucket=%d, split=%d, level=%d\n",
		hi?"^":"v", cnt, idx, split, level);

	//< string is not in the table: 		//< create it:
	++cnt;mem += rec_len;					//< increment counters
	B=malloc(rec_len);chk(B,NULL);			//< create a new record
	B->h              = hash;				//< set djb value
	B->n              = n;					//< set strlen
	B->usage          = idx;				//< set usage
	*dsn(B->s,s,n)    = 0;					//< store string and terminate it
	B->next           = buckets[idx];		//< link existing list item if any
	buckets[idx]      = B;					//< put at the head of the list

	
	O("\n");T(WARN, "SET -> %s", B->s);
	hsh_print(B);

	C rnd = 0;
	if(B->next){
		DO(3,
		O("HAVE A NEXT, DOING ROUND %d\n", ++rnd);
		bkt bp = buckets[split];
		I newBucket = split+level;
		W(bp){
			if((bp->h&((level<<1)-1))==newBucket){
				bkt tmp=bp;
				bp=bp->next;
				tmp->next=buckets[newBucket];
				buckets[newBucket] = tmp;
			} else bp=bp->next;
		}
		if(++split==level){
			level <<= 1;
			split = 0;
			buckets=(bkt*)realloc((G*)buckets,SZ(bkt)*2*(UJ)level);
			T(WARN,"realloc: lvl=%d buckets=%d bytes=%d", level, 2*(UJ)level, 2*level*SZ(bkt));
			chk(buckets,NULL);
			DO(level,buckets[level+i]=0)
		}
	)}
	R B->s; //< ptr on string
}

I hsh_init() {
	LOG("hsh_init");
	X(buckets, T(WARN, "hash table is already initialized"), 1);
	buckets=(bkt*)calloc(2*level,SZ(bkt)); //< initialize hash table
	T(WARN," calloc: lvl=%d buckets=%d bktsize=%d bytes=%d", level, 2*level, SZ(bkt), 2*level*SZ(bkt));
	chk(buckets,1);
	R 0;	
}

ZI hsh_test() {
	LOG("hsh_test");

	hsh_init();

	hsh_get("FKTABLE_CAT");
	hsh_get("cov");
	hsh_get("bmp");
	hsh_get("frameset");
	hsh_get("cos");
	hsh_get("fmt");
/*
	hsh_get("FKTABLE_CAT");
	hsh_get("cov");
	hsh_get("bmp");
	hsh_get("frameset");
	hsh_get("cos");
	hsh_get("fmt");
*/
	bkt b;
	UJ c=0;
	O("dump table, level=%d\n", level);//, level*2);
	DO(level*2,
		b=buckets[i];
		if(b)O("\n");
		W(b){
			O("%lu %s ", i, b->s);
			b=b->next;
			c++;
		}
	);
	O("\n=%lu\n",c);
	R 0;
}

#ifdef RUN_TESTS
I main(){R hsh_test();}
#endif

//:!~
