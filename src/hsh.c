//!\file hsh.c \brief simple hash table with separate chaining

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "___.h"
#include "trc.h"
#include "arr.h"
#include "hsh.h"

//! random string
S rndstr(sz size){
	LOG("rndstr");
	const C charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	const H dictlen = 62;
	S r = (S)malloc(size);chk(r,NULL);
	DO(size,
		I key = rand()%(I)(dictlen);
		r[i] = charset[key];
	)
	r[size] = '\0';
	R r;
}

//! djb for president \see http://www.burtleburtle.net/bob/hash/doobs.html
I djb(G* a,UI n){I h=5381;DO(n,h=33*(h^a[i]))R h;}

//! copy with seek \param d dest \param s source \param n len
S dsn(V* d, V* s, UJ n){R(S)memcpy(d,s,n)+n;}

V hsh_print(BKT b) {
	LOG("hsh_print");
	T(TEST, "idx  -> %d",   b->idx);	
	T(TEST, "s    -> %s",   b->s);
	T(TEST, "hsh  -> %d",   b->h);
	T(TEST, "len  -> %d",   b->n);
	T(TEST, "next -> %d",   (b->next)?(b->next)->idx:-1);
}

sz hsh_capacity(HT ht){
	R ht->level * 2;
}

//! \brief lookup str in the hash table \param s str
//! \return ptr to permanent address
//! \see https://github.com/twonds/ejabberd/blob/master/apps/ejabberd/c_src/tls_drv.c
S hsh_get(HT ht, S s){
	LOG("hsh_get");
	I n = scnt(s),
	rec_len = SZ_BKT+n+1,
	   hash = djb((G*)s,n), //< calculate the hash
		idx = hash&(ht->level - 1); //< map to the first half of table

	if(idx < ht->split)
		idx = hash&((ht->level<<1)-1); //< map to the entire table

	//! lookup the value
	BKT B = ht->buckets[idx];
	I depth = 0;
	W(B){									//< inspect the linked list from head
		if(B->n==n){						//< if length matches...
	 		DO(n,if(B->s[i]!=s[i])goto L0)	//< compare bytes, L0 on first mismatch
	 		T(TEST, "GET <-- %s (depth=%d)", B->s, depth);
	 		//hsh_print(B);
	 		R B->s;}						//< found a match, return the ptr				
		L0:depth++;B=B->next;}				//< move to next linked node
										
	T(TRACE, "vcnt=%lu bucket=%d, split=%d, level=%d",
		ht->vcnt, idx, ht->split, ht->level);

	//! value is not in the table, insert it:
	B=malloc(rec_len);chk(B,NULL);			//< init new value
	ht->bcnt += !!ht->buckets[idx];			//< if bkt is empty, increment bcnt
	ht->mem += rec_len;	ht->vcnt++;			//< increment odometers
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
					ht->bcnt -= !!*bp; //< if source is empty, decrement bcnt
					ht->bcnt += !!ht->buckets[new_idx]; //< if target is empty, increment bcnt
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
				DO(ht->level, ht->buckets[ht->level + i]=NULL) //< zero out
				T(TRACE, "realloc: lvl=%d buckets=%lu bytes=%lu",
					ht->level, hsh_capacity(ht), cap * SZ(BKT));
			}
		)
	}
	R B->s; //< ptr to string
}

sz hsh_mem(HT ht) {
	sz ht_size = SZ(SZ_HT) + SZ(BKT) * hsh_capacity(ht); //< own size
	R ht->mem + ht_size;
}

E hsh_load_factor(HT ht) {
	return (E)ht->bcnt / hsh_capacity(ht);
}

HT hsh_init(I split, I level) {
	LOG("hsh_init");
	HT ht = (HT)malloc(SZ_HT);chk(ht,NULL);
	ht->split = split;
	ht->level = level;
	ht->bcnt = ht->vcnt = ht->mem = 0;
	sz init_size = hsh_capacity(ht);
	ht->buckets = (BKT*)malloc(init_size * SZ(BKT)); //< initialize hash table
	chk(ht->buckets,NULL);
	DO(init_size, ht->buckets[i]=NULL) // zero out
	T(WARN,"calloc: lvl=%d buckets=%d bktsize=%d bytes=%d vcnt=%lu bcnt=%lu",
		ht->level, hsh_capacity(ht), SZ(BKT), hsh_capacity(ht) * SZ(BKT), ht->vcnt, ht->bcnt);
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

ZI hsh_test() {
	LOG("hsh_test");

	HT ht = hsh_init(0,2);

	//! insert
	hsh_get(ht, "FKTABLE_CAT");
	hsh_get(ht, "cov");
	hsh_get(ht, "bmp");
	hsh_get(ht, "frameset");
	hsh_get(ht, "cos");
	hsh_get(ht, "fmt");

	//! lookup
	hsh_get(ht, "FKTABLE_CAT");
	hsh_get(ht, "cov");
	hsh_get(ht, "bmp");
	hsh_get(ht, "frameset");
	hsh_get(ht, "cos");
	hsh_get(ht, "fmt");

	DO(1000, S s = rndstr(3); hsh_get(ht, s); free(s);)

	BKT b;
	UJ c = 0;
	T(TEST,"dump table, level=%d", ht->level);//, level*2);

	DO(hsh_capacity(ht),
		b=ht->buckets[i];
		TSTART();
		T(TEST, "%5lu ", i);
		W(b){
			T(TEST,"%s %s ", b->s, STR_ARROW_RIGHT);
			b = b->next;
			c++;
		}
		T(TEST, STR_EMPTY_SET);
		TEND();
	);
	T(TEST, "capacity=%lu, vcnt=%lu, bcnt=%lu, bytes=%lu, lfactor=%.3f", hsh_capacity(ht), ht->vcnt, ht->bcnt, hsh_mem(ht), hsh_load_factor(ht));

	hsh_destroy(ht);
	R0;
}

#ifdef RUN_TESTS_HSH
I main(){R hsh_test();}
#endif

//:!~
