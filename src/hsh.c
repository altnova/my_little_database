//!\file hsh.c \brief simple hash table with separate chaining

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "___.h"
#include "trc.h"
#include "arr.h"
#include "rnd.h"
#include "hsh.h"

//! djbhash \see http://www.burtleburtle.net/bob/hash/doobs.html
Z inline I djb(G* a,UI n){I h,r=5381;DO(n,h=33*(h^a[i])); R h;}

//! copy with seek \param d dest \param s source \param n len
S dsn(V* d, V* s, UJ n){R(S)memcpy(d,s,n)+n;}

Z inline UJ hsh_idx(HT ht, S s, I n, I*h) {
	I hash = djb((G*)s,n); //< calculate the hash
	UJ idx = hash&(ht->level - 1); //< map to the first half of table
	if(idx < ht->split)
		idx = hash&((ht->level<<1)-1); //< map to the entire table
	*h = hash;
	R idx;
}

V hsh_print(BKT b) {
	LOG("hsh_print");
	T(TEST, "idx  -> %d",   b->idx);	
	T(TEST, "s    -> %s",   b->s);
	T(TEST, "hsh  -> %d",   b->h);
	T(TEST, "len  -> %d",   b->n);
	T(TEST, "next -> %d",   (b->next)?(b->next)->idx:-1);
}

S hsh_get(HT ht, S s){
	LOG("hsh_get");
	I hash;
	UJ n = scnt(s), idx = hsh_idx(ht, s, n, &hash);

	//! lookup the value
	BKT*b = &ht->buckets[idx];
	I depth = 0;
	W(*b){									//< inspect the linked list from head
		if((*b)->n==n){						//< if length matches...
	 		DO(n,if((*b)->s[i]!=s[i])goto L0)	//< compare bytes, L0 on first mismatch
	 		T(TEST, "GET <-- %s (depth=%d)", (*b)->s, depth);
	 		//hsh_print(B);
	 		R (*b)->s;}						//< found a match, return the ptr				
		L0:depth++;b=&(*b)->next;}				//< move to next linked node

	R NULL;
}

//! \brief insert str into the hash table \param ht,s table, str
//! \return ptr to permanent address or NULL if error
//! \see https://github.com/twonds/ejabberd/blob/master/apps/ejabberd/c_src/tls_drv.c
S hsh_ins(HT ht, S s){
	LOG("hsh_ins");
	S r = hsh_get(ht, s);
	P(r, r); //< return pointer if found

	I hash;
	UJ n = scnt(s),
		rec_len = SZ_BKT+n+1,
		idx = hsh_idx(ht, s, n, &hash);

	T(TRACE, "cnt=%lu bucket=%d, split=%d, level=%d",
		ht->cnt, idx, ht->split, ht->level);

	//! value is not in the table, insert it:
	BKT B = malloc(rec_len);chk(B,NULL);	//< init new value
	ht->mem += rec_len;	ht->cnt++;			//< increment odometers
	B->h              = hash;				//< set hash value
	B->n              = n;					//< set payload length
	B->idx            = idx;				//< set current bucket index
	B->next           = ht->buckets[idx];	//< link existing list item, if any
	*dsn(B->s,s,n)    = 0;					//< copy payload and terminate it
	ht->buckets[idx]  = B;					//< put at the head of the list

	T(TEST, "INS --> %s, has_tail=%d", B->s, !!B->next);//hsh_print(B);
	//! if inserted bucket has a tail, it is a good time to
	//! attempt to split tails of buckets at split, split+1, split+2...
	//! by moving linked items to the next available bucket \c new_idx
	//! located in the upper part of the table.
	if(B->next){
		DO(3,
			BKT*bp = &ht->buckets[ht->split]; //< head of the list
			UJ new_idx = ht->level + ht->split; //< next available upper idx
			W(*bp){ //< while list continues:
				//! if item's upper hash lands on target index, move it there:
				if(((*bp)->h&((ht->level<<1)-1))==new_idx){
					BKT moved = *bp; //< item to move
					*bp=(*bp)->next; //< shift head of the list
					moved->next = ht->buckets[new_idx]; //< link existing list item, if any
					ht->buckets[new_idx] = moved; //< put at the head of the list
					T(TEST, "MOV --> %s (%lu -> %lu)", moved->s, moved->idx, new_idx);
					moved->idx = new_idx; //< update idx
				} else bp=&(*bp)->next; //< keep walking the list
			}
			//! once split reaches the middle:
			if(++ht->split==ht->level){ 
				ht->level <<= 1; //< double the size of the table
				ht->split = 0; //< rewind split
				sz cap = hsh_capacity(ht);
				ht->buckets = (BKT*)realloc((G*)ht->buckets, cap * SZ(BKT));
				chk(ht->buckets,NULL);
				DO(ht->level, ht->buckets[ht->level + i]=NULL) //< zero new buckets
				T(TEST, "REA -*- %d", hsh_capacity(ht));
			}
		)
	}
	R B->s; //< ptr to string
}

inline sz hsh_mem(HT ht) {
	sz ht_size = SZ(SZ_HT) + SZ(BKT) * hsh_capacity(ht); //< own size
	R ht->mem + ht_size;
}

E hsh_factor(HT ht) {
	R (E)ht->cnt / hsh_capacity(ht);
}

HT hsh_init(I level) {
	LOG("hsh_init");
	HT ht = (HT)malloc(SZ_HT);chk(ht,NULL);
	ht->level = level;
	ht->split = ht->cnt = ht->mem = 0; //< init odometers
	sz init_size = hsh_capacity(ht);
	ht->buckets = (BKT*)calloc(init_size, SZ(BKT)); //< initialize hash table
	chk(ht->buckets,NULL);
	T(TEST,"calloc: lvl=%d buckets=%d bktsize=%d bytes=%d",
		ht->level, hsh_capacity(ht), SZ(BKT), hsh_capacity(ht) * SZ(BKT));
	R ht;
}

V hsh_destroy(HT ht){
	LOG("hsh_destroy");
	BKT curr, next;
	UJ c = 0;
	DO(hsh_capacity(ht),
		curr = ht->buckets[i];
		//T(TEST, "freeing up bkt=%lu (%d)", i, !!curr);
		W(curr) {
			next = curr->next;
			free(curr);
			curr = next;
			c++;
		}
	)
	free(ht->buckets);
	free(ht);
	T(TEST, "released %lu values, hash table destroyed", c);
}

V hsh_dump(HT ht) {
	LOG("hsh_dump")
	BKT*b;
	DO(hsh_capacity(ht),
		b = &ht->buckets[i];
		if(!*b)continue;
		TSTART();
		T(TEST, "%5lu ", i);
		W(*b){
			T(TEST,"%s %s ", (*b)->s, STR_ARROW_RIGHT);
			b = &(*b)->next;
		}
		T(TEST, STR_EMPTY_SET);
		TEND();
	);
	T(TEST, "capacity=%lu, cnt=%lu, bytes=%lu, lfactor=%.3f",
		hsh_capacity(ht), ht->cnt, hsh_mem(ht), hsh_factor(ht));
}

ZI hsh_test(sz rand_cnt, sz rand_len) {
	LOG("hsh_test");

	HT ht = hsh_init(2);

	S keys[] = { "FKTABLE_CAT", "cov", "bmp", "frameset", "cos", "fmt" }; 

	//! insert
	DO(6, hsh_ins(ht, keys[i]));
	//! lookup
	DO(6, hsh_get(ht, keys[i]));
	//! rand
	DO(rand_cnt,
		S s = (S)malloc(rand_len+1);chk(s,1);
		hsh_ins(ht, rnd_str(s,rand_len,CHARSET_AZ));
		free(s))

	hsh_dump(ht);

	hsh_destroy(ht);
	R0;
}

#ifdef RUN_TESTS_HSH
I main(){R hsh_test(10, 3);}
#endif

//:!~
