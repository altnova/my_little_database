//!\file hsh.c \brief simple hash table with separate chaining

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "___.h"
#include "trc.h"
#include "arr.h"
#include "rnd.h"
#include "hsh.h"
#include "clk.h"

//! djbhash \see http://www.burtleburtle.net/bob/hash/doobs.html
Z inline HTYPE hsh_djb(V*a,UJ n){HTYPE h=5381;DO(n,h=33*(h^((G*)a)[i]));R h;}
HTYPE hsh_identity(V*a,UJ n){R(I)a;}

//! copy with seek \param d dest \param s source \param n len
ZS dsn(V* d, V* s, UJ n){R memcpy(d,s,n)+n;}

Z inline V hsh_idx(HT ht, V*s, UJ n, HTYPE*h, HTYPE*idx) {
	LOG("hsh_idx");
	HTYPE hash = ht->fn(s,n); //< calculate the hash
	HTYPE i = hash&(ht->level - 1); //< map to the first half of table
	C hi = 0;
	if(i < ht->split){
		hi = 1;
		i = hash&((ht->level<<1) - 1); //< map to the entire table
	}
	//T(TEST, "IDX %s --> idx(%d)->%d (hi=%d)", s, hash, i, hi);
	*h = hash;
	*idx = i;
}

//! print value
V hsh_print(BKT b) {
	LOG("hsh_print");
	T(TEST, "bkt     -> %d",   b);
	T(TEST, "idx     -> %d",   b->idx);	
	T(TEST, "s       -> %s",   b->s);
	T(TEST, "h       -> %d",   b->h);
	T(TEST, "n       -> %d",   b->n);
	T(TEST, "payload -> %lu",  b->packed);	
	//T(TEST, "packed -> %d",   b->packed);	
	//T(TEST, "next   -> %d",   (b->next)?(b->next)->idx:-1);
	//T(TEST, "next   -> %lu",   b->next);
}

BKT hsh_get_bkt(HT ht, V*k, sz n){
	LOG("hsh_get");
	HTYPE hash, idx;
	P(!k||!n,NULL); //< null ptr or empty key
	hsh_idx(ht, k, n, &hash, &idx);

	//! lookup the value
	BKT*b = &ht->buckets[idx];
	I depth = 0;
	W(*b){									//< inspect the linked list from head
		if((*b)->n==n){						//< if length matches...
	 		DO(n,if(((G)(*b)->s[i])!=((G*)k)[i])goto NEXT)	//< compare bytes, L0 on first mismatch
	 		//T(TEST, "GET <-- %s (depth=%d)", (*b)->s, depth);
	 		//hsh_print(B);
	 		R(*b);}							//< found a match, return the ptr				
		NEXT:depth++;b=&(*b)->next;}		//< move to next linked node

	R NULL;
}

V* hsh_get_payload(HT ht, V*k, sz n){
	BKT r = hsh_get_bkt(ht, k, n);
	P(!r,NULL);
	R r->payload;
}

V* hsh_get(HT ht, V*s, sz n) {
	BKT r = hsh_get_bkt(ht, s, n);
	P(!r,NULL);
	R r->s;
}

//! \see https://github.com/twonds/ejabberd/blob/master/apps/ejabberd/c_src/tls_drv.c
BKT hsh_ins(HT ht, V*k, sz n, V*payload){
	LOG("hsh_ins");
	P(!k||!n,NULL); //< null ptr or empty key
	V*r = hsh_get_bkt(ht, k, n);
	P(r, r); //< return pointer if found

	HTYPE hash, idx;
	UJ rec_len = SZ_BKT + n + 1;
	hsh_idx(ht, k, n, &hash, &idx);

	T(TRACE, "cnt=%lu bucket=%d, split=%d, level=%d",
		ht->cnt, idx, ht->split, ht->level);

	//! value is not in the table, insert it:
	BKT B = malloc(rec_len);chk(B,NULL);	//< init new value
	ht->mem += rec_len;	ht->cnt++;			//< increment odometers
	B->h              = hash;				//< set hash value
	B->n              = n;					//< set val length
	B->idx            = idx;				//< set current bucket index
	B->payload		  = payload;			//< set payload pointer
	B->packed		  = 0;					//< not in heap
	B->next           = ht->buckets[idx];	//< link existing list item, if any
	*dsn(B->s,k,n)    = 0;					//< copy val and terminate it
	
	ht->buckets[idx]  = B;					//< put at the head of the list

	//T(TEST, "INS %s --> (%d)", B->s, idx);//hsh_print(B);
	//! if inserted bucket has a tail, it is a good time to
	//! attempt to split tails of buckets at split, split+1, split+2...
	//! by moving linked items to the next available bucket \c new_idx
	//! located in the upper part of the table.
	if(B->next){
		DO(ht->rounds,
			BKT*bp = &ht->buckets[ht->split]; //< head of the list
			HTYPE new_idx = ht->split + ht->level; //< next available upper idx
			W(*bp){ //< while list continues:
				//! if item's upper hash lands on target index, move it there:
				if(((*bp)->h&((ht->level<<1)-1))==new_idx){
					BKT moved = *bp; //< item to move
					*bp=(*bp)->next; //< shift head of the list
					moved->next = ht->buckets[new_idx]; //< link existing list item, if any
					ht->buckets[new_idx] = moved; //< put at the head of the list
					//T(TEST, "MOV --> %s (%d -> %d)", moved->s, moved->idx, new_idx);
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
				//T(TEST, "REA -*- %d", hsh_capacity(ht));
			}
		)
	}
	R B; //< bucket
}

sz hsh_mem(HT ht) {
	sz ht_size = SZ(SZ_HT) + SZ(BKT) * hsh_capacity(ht); //< own size
	R ht->mem + ht_size;
}

Z HTYPE hsh_bcnt(HT ht) {
	HTYPE r = 0;
	DO(hsh_capacity(ht), r+=!!ht->buckets[i])
	R r;
}

E hsh_factor(HT ht) {
	R (E)ht->cnt / hsh_capacity(ht);
}

E hsh_bavg(HT ht) {
	R (E)ht->cnt / hsh_bcnt(ht);
}

HT hsh_init(I level, H split_rounds) {
	R hsh_init_custom(level, split_rounds, hsh_djb);
}

HT hsh_init_custom(I level, H split_rounds, HSH_FN fn) {
	LOG("hsh_init");
	X(level<2, T(WARN, "level can't be less than 2"), NULL);
	X((level&(level-1)), T(WARN, "level must be a power of 2"), NULL);
	X(split_rounds<1, T(WARN, "split_rounds can't be less than 1"), NULL);
	
	HT ht = (HT)malloc(SZ_HT);chk(ht,NULL);
	ht->level = level;
	ht->rounds = split_rounds;
	ht->heap = NULL;
	ht->fn = fn;
	ht->split = ht->cnt = ht->mem = 0; //< init odometers
	HTYPE init_size = hsh_capacity(ht);
	ht->buckets = (BKT*)calloc(init_size, SZ(BKT)); //< initialize hash table
	chk(ht->buckets,NULL);
	T(DEBUG,"calloc: level=%d capacity=%d mem=%d",
		ht->level, hsh_capacity(ht), hsh_capacity(ht) * SZ(BKT));
	R ht;
}

C hsh_pack(HT ht) {
	LOG("hsh_pack");
	BKT prev, curr, next;
	V*tmpheap = malloc(ht->mem);
	chk(tmpheap, 0);
	UJ hptr = 0; //< heap pointer
	V*mc;
	DO(hsh_capacity(ht),
		prev = NULL;
		curr = ht->buckets[i];
		W(curr){	
			//hsh_print(curr);
			sz bsize = SZ_BKT + curr->n + 1;
			BKT old_addr = curr;
			BKT new_addr = tmpheap + hptr;
			mc = mcpy(new_addr, curr, bsize);
			if(!prev)
				ht->buckets[i] = new_addr;
			else
				prev->next = new_addr;
			prev = new_addr;
			next = curr->next;
			if (!new_addr->packed) {
				free(old_addr);
				new_addr->packed = 1;
			}
			curr = next;
			hptr += bsize;
			//T(TEST, "packed bkt=%lu (size=%lu)", i, bsize);
		}
	)
	if(ht->heap)free(ht->heap);
	ht->heap = tmpheap;
	//T(TEST, "packing complete");
	R1;
}

V hsh_destroy(HT ht) {
	LOG("hsh_destroy");
	BKT curr, next;
	HTYPE c = 0;
	DO(hsh_capacity(ht),
		curr = ht->buckets[i];
		W(curr){
			next = curr->next;
			if(!curr->packed){
				//T(TEST, "freeing up bkt=%lu %s (%d)", i, curr->s, curr->packed);
				free(curr);
			}
			curr = next;
			c++;
		}
	)
	if(ht->heap)free(ht->heap);
	free(ht->buckets);
	free(ht);
	T(DEBUG, "released %lu values, hash table destroyed", c);
}

V hsh_info(HT ht) {
	LOG("hsh_info");
	T(INFO, "cap=%8d, cnt=%8d, bcnt=%8d, bavg=%4.2f, lf=%4.2f, spl=%8d, mem=%8lu",
		hsh_capacity(ht), ht->cnt, hsh_bcnt(ht), hsh_bavg(ht), hsh_factor(ht), ht->split, hsh_mem(ht));
}

V hsh_dump(HT ht) {
	LOG("hsh_dump")
	BKT b;
	DO(hsh_capacity(ht),
		b = ht->buckets[i];
		if(!b)continue;
		TSTART();
		T(TEST, "%5lu ", i);
		W(b){
			//hsh_print(*b);
			T(TEST,"(%s)=%d,%c %s ", b->s, b->n, "NY"[b->packed], STR_ARROW_RIGHT);
			b = b->next;
		}
		T(TEST, STR_EMPTY_SET);
		TEND();
	);

	hsh_info(ht);
}

V hsh_each(HT ht, HT_EACH fn, V*arg) {
	LOG("hsh_each")
	BKT b;
	UJ cnt = 0;
	DO(hsh_capacity(ht),
		b = ht->buckets[i];
		if(!b)continue;
		W(b){
			fn(b, arg, cnt++);
			b = b->next;
		}
	)
}

#ifdef RUN_TESTS_HSH

V hsh_test_each_fn(BKT bkt, V*arg, HTYPE i) {
	Arr c = (Arr)arg;
	arr_add(c, bkt->n);
}

C hsh_test_insert_rand(HT ht, UJ cnt, UJ rand_len) {
	LOG("hsh_test_insert_rand");
	DO(cnt,
		S s = (S)malloc(rand_len+1);chk(s,1);
		hsh_ins(ht, rnd_str(s,rand_len,CHARSET_AZ), rand_len, NULL);
		free(s))	
	R0;
}

//! dummy walk for testing data locality speedup
UJ hsh_walk(HT ht) {
	LOG("hsh_walk")
	BKT b;
	UJ res = 0;
	DO(hsh_capacity(ht),
		b = ht->buckets[i];
		if(!b)continue;
		W(b){
			res += !!hsh_get(ht, b->s, b->n);
			b = b->next;
		}
	);
	R res;
}

ZI hsh_test(sz rand_cnt, sz rand_len) {
	LOG("hsh_test");

	HT ht = hsh_init();

	S keys[] = { "FKTABLE_CAT", "cov", "bmp", "frameset", "cos", "fmt" }; 
	I keys_len = 6;

	//! test insert
	BKT*addrs[keys_len];
	DO(keys_len, addrs[i]=hsh_ins(ht, keys[i], scnt(keys[i]), 0));

	//! test insert #2
	DO(keys_len,
		X(addrs[i]!=hsh_ins(ht, keys[i], scnt(keys[i]), 0),
			T(TEST, "unstable pos: %d", i),
			1)
	);

	hsh_test_insert_rand(ht, rand_cnt, rand_len);

	//! test lookup
	DO(keys_len,
		S found = hsh_get(ht, keys[i], scnt(keys[i]));
		X(!found,
			{hsh_dump(ht);T(FATAL, "expected %s, got NULL", keys[i]);},
			1);
	)

	//! test each
	Arr out = arr_init(1, UJ);
	hsh_each(ht, (HT_EACH)hsh_test_each_fn, (V*)out);
	TSTART();
	T(TEST, "arr_each result -> ");
	DO(ht->cnt,
		T(TEST, " (%lu)", *arr_at(out,i,UJ)))
	TEND();
	arr_free(out);

	hsh_dump(ht);
	hsh_pack(ht); //< test inital pack
	hsh_pack(ht); //< test dummy repack
	hsh_test_insert_rand(ht, 1, rand_len);
	hsh_pack(ht); //< test repack mixed
	hsh_dump(ht);

	clk_start();
	UJ N = 2000000, t,t1, r;
	hsh_test_insert_rand(ht, N, rand_len);
	T(TEST, "inserted %lu values \t--> %lums", N, clk_stop());
	r = hsh_walk(ht);
	T(TEST, "walk %lu unpacked values, r=%lu \t--> %lums", ht->cnt, r, t=clk_stop());

	hsh_pack(ht);
	T(TEST, "packed %lu values \t--> %lums", ht->cnt, clk_stop());
	r = hsh_walk(ht);
	t1 = clk_stop();
	E speedup = (E)t/t1;
	T(TEST, "walk %lu packed values, r=%lu \t--> %lums", ht->cnt, r, t1);
	//T(TEST, "packing speedup \e[91m%.2f%%\e[0m", 100*speedup);
	T(TEST, "packing speedup \e[91m%.2f%%\e[0m", 100*speedup);

	hsh_info(ht);

	r = ht->cnt;
	hsh_destroy(ht);

	T(TEST, "destroyed %lu values \t--> %lums", r, clk_stop());

	R0;
}

I main(){R hsh_test(1, 5);}
#endif

//:!~
