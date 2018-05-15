//! \file tok.c \brief string tokenizer

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "___.h"
#include "cfg.h"
#include "trc.h"
#include "fio.h"
#include "hsh.h"
#include "idx.h"
#include "clk.h"
#include "arr.h"
#include "stm.h"
#include "usr.h"

#define PACK_EVERY_N_RECS (4*4096)
#define WORDBAG_INIT_SIZE 1048576

Z HT stopwords;	//< hash table: stop words
Z HT ftidx[FTI_FIELD_COUNT]; //< hash tables: stem -> Arr(rec_ids)
Z HT wordbags[FTI_FIELD_COUNT]; //< hash tables: term -> stem

Z V* wordbag_heaps[FTI_FIELD_COUNT];
Z sz wordbag_heap_sizes[FTI_FIELD_COUNT];
Z sz wordbag_heap_usage[FTI_FIELD_COUNT];

//Z inline V tok_lcase(S tk) {DO(scnt(tk), tk[i]=tolower(tk[i]));}

UJ tok_load_stop_words(S fname) {
	LOG("tok_load_stop_words");
	FILE* fd;
	xfopen(fd, fname, "r+", NIL);
	UJ fsz = fsize(fd);
	S buf = malloc(fsz); chk(buf,NIL);
	UJ bytesRead = fread(buf, 1, fsz, fd);
	lcse(buf);
	S delim = "\n";
	stok(buf, fsz, "\n",
		T(TEST, "stopword --> (%s)", tok);
		hsh_ins(stopwords, tok, tok_len, NULL))
	fclose(fd);
	free(buf);

	hsh_pack(stopwords);
	hsh_info(stopwords); //hsh_dump(stopwords);

	R stopwords->cnt;
}

//! comparator for qsort()
ZI tok_cmp_qsort(const V*a, const V*b) {
	R ((UJ)a)-((UJ)b);
}

//! sort index by rec_id
ZV tok_ftidx_sort_each(BKT bkt, V*arg, UJ i) {
	LOG("tok_ftidx_sort_each");
	Arr a = (Arr)bkt->payload;
	if(a->used<2)R;
	qsort(a->data, a->used, SZ(ID), tok_cmp_qsort);
	//T(TEST, "%lu %s index sorted %lu items", i, bkt->s, a->used);
}

V zero(G*a,sz len) {
	DO(len, a[i]=0)
}

V tok_wordbag_adjust_ptr(BKT bkt, UJ diff, HTYPE i) {
	bkt->payload -= diff;
}

V tok_search(S query) {
	LOG("tok_search");
	lcse(query); //< lowercase
	//T(TEST, "raw query: -> (%s)", query);
	TSTART();
	T(TEST, "stemmed search terms: ");
	stok(query, scnt(query), FTI_TOKEN_DELIM,
		//for(I j=0;j<255;j++)if(D[j]>0)O("(%d=%c)",j, D[j]);
		//if((tok_len<2||hsh_get(stopwords, tok, tok_len)))continue;
		//! apply stemmer
		//T(TEST, "(%s ->", tok);
		//T(TEST, "(%s ->", tok);
		tok_len = stm(tok, 0, tok_len-1)+1;
		tok[tok_len] = 0; //< include null
		T(TEST, "(%s) ", tok);
	)
	TEND();
}

const ZI STOK_TRACE=0, STOK_VAL_LEN=5; ZS STOK_VAL = "aaron";
UJ tok_index_field(ID rec_id, I field, S s, UJ i) {
	LOG("tok_index_field");
	lcse(s); //< lowercase
	I tok_cnt = 0;
	stok(s, scnt(s), FTI_TOKEN_DELIM,
    	V* wbh = wordbag_heaps[field];
    	sz wbh_sz = wordbag_heap_sizes[field];
    	sz wbh_used = wordbag_heap_usage[field];

    	if((tok_len<2||hsh_get(stopwords, tok, tok_len)))continue;

		//if(STOK_TRACE&&!mcmp(tok,STOK_VAL, STOK_VAL_LEN))
		//	T(TEST, "TOK %s -> (%s %d)", STOK_VAL, tok, tok_len);
    	
		BKT b = hsh_ins(wordbags[field], tok, tok_len, 0);
		C from_wordbag = 0;
		if(!b->payload) {
			//! apply stemmer
			tok_len = stm(tok, 0, tok_len-1)+1;
			tok[tok_len] = 0;

			if(STOK_TRACE&&!mcmp(tok,STOK_VAL,STOK_VAL_LEN))
				T(TEST, "STM %s -> (%d)", tok, tok_len);

			//! grow heap if needed
	    	if(wbh_used+tok_len+1>wbh_sz){
	    		V*old_wbh = wbh;
				wordbag_heaps[field] = wbh = realloc(wbh, 2 * wbh_sz);chk(wbh,NIL);
				T(TEST, "realloc heap %lu, diff=%lu", 2 * wbh_sz, old_wbh-wbh);
				zero(wbh+wbh_sz,wbh_sz); //< zero out
				hsh_each(wordbags[field], tok_wordbag_adjust_ptr, old_wbh-wbh);
				wbh_sz = wordbag_heap_sizes[field] *= 2;
	    	}
	    	//! store stem in heap
			mcpy(wbh+wbh_used, tok, tok_len+1);
			wordbag_heap_usage[field] += tok_len+1;
			//! link stem to word
			b->payload = wbh+wbh_used;
		} else {
			//T(TEST, "got stem: %s -> %s", tok, b->payload);
			tok = b->payload;
			from_wordbag = 1;
		}
    	//continue;
		if(STOK_TRACE&&!mcmp(tok,STOK_VAL,STOK_VAL_LEN))
			T(TEST, "INS %s -> (%s %d) from_bag=%d", STOK_VAL, tok, tok_len, from_wordbag);

		tok[tok_len+1]=0;
    	b = hsh_ins(ftidx[field], tok, tok_len+1, 0);


		if(!b->payload)
			b->payload = arr_init(10, ID);

		arr_add(b->payload, rec_id);
		tok_cnt++;
	)

	if(((I)i%PACK_EVERY_N_RECS)==0) {
		clk_start();
		hsh_pack(ftidx[field]);
		hsh_pack(wordbags[field]);
		T(DEBUG, "packed in %lums", clk_stop());
	}

	R tok_cnt;
}

UJ tok_index_rec(Rec r, V*arg, UJ i) {
	//rec_print_dbg(r);
	tok_index_field(r->rec_id, fld_publisher, r->publisher, i);
	tok_index_field(r->rec_id, fld_title, r->title, i);
	tok_index_field(r->rec_id, fld_author, r->author, i);
	tok_index_field(r->rec_id, fld_subject, r->subject, i);
	R0;
}

V tok_ftidx_repack_each(BKT bkt, V*arg, HTYPE i) {
	HT wordbag = (HT)(bkt->payload);
	hsh_pack(wordbag);//hsh_info(wordbag);
}

V tok_pack() {
	LOG("tok_pack");
	clk_start();
	DO(FTI_FIELD_COUNT,
		hsh_each(ftidx[i], tok_ftidx_repack_each, NULL);
		hsh_pack(ftidx[i]);//hsh_info(ftidx[i]);
	)
	T(TEST, "packed indexes in %lums", clk_stop());
}

V tok_ftidx_destroy_each(BKT bkt, V*arg, HTYPE i) {
	//HT wordbag = (HT)bkt->payload;
	//hsh_destroy(wordbag);
	Arr a = (Arr)bkt->payload;
	arr_free(a);
}

V tok_wordbag_inspect_each(BKT bkt, V*arg, HTYPE i) {
	LOG("tok_wordbag_inspect_each");
	UJ count = *bkt->s;
	T(TEST, "%s (%d=%s)", bkt->s, count, bkt->payload);
}

V tok_ftidx_inspect_each(BKT bkt, V*arg, HTYPE i) {
	LOG("tok_ftidx_inspect_each");
	Arr wordbag = (Arr)bkt->payload;
	//TSTART();
	//if(arr_sz(wordbag)>1000) {
	if(STOK_TRACE&&!mcmp(bkt->s, "aaro",4))
		T(TEST, "(%s) %lu %d", bkt->s, arr_sz(wordbag), bkt->h);
}

I main() {
	LOG("tok_test");

	//! test tokenizer
	S c = "a aaron abaissiez abandon aaron aaron abandoned abase abash abate abated abatement abatements abates abbess abbey abbeys abbominable abbot a";
	C qq[scnt(c)]; mcpy(qq,c,scnt(c));
	//tok_search(qq);//exit(0);
	
	db_init(DAT_FILE, IDX_FILE);

	DO(FTI_FIELD_COUNT,
		ftidx[i]=hsh_init(2,3);
		wordbags[i]=hsh_init(2,3);
		wordbag_heaps[i]=malloc(WORDBAG_INIT_SIZE);chk(wordbag_heaps[i],1);
		wordbag_heap_sizes[i]=WORDBAG_INIT_SIZE;
		wordbag_heap_usage[i]=0;
		X(!ftidx[i], T(FATAL, "cannot initialize hash table %d", i), 1);
	)

	//! load stop words
	stopwords = hsh_init(2,3);
	UJ swcnt = tok_load_stop_words("dat/stopwords.txt");
	X(swcnt==NIL,T(FATAL, "cannot load stopwords"), 1);
	T(TEST, "loaded %lu stopwords", stopwords->cnt);

	exit(0);

	//! test tokenizer
	//mcpy(qq,c,scnt(c));tok_index_field(0, 5, qq, 0); exit(0);

	//! build inverted index
	clock_t idx_start = clk_start();
	UJ res = idx_each(tok_index_rec, NULL, 1);
	X(res==NIL,T(FATAL, "unable to index records"), 1);
	T(TEST, "indexed %lu records in %lums", res, clk_diff(idx_start, clk_start()));

	//! sort document buckets
	DO(FTI_FIELD_COUNT,hsh_each(ftidx[i], tok_ftidx_sort_each, NULL))
	T(TEST, "sorted buckets in %lums", clk_stop());

	T(TEST, "fti metrics:");
	DO(FTI_FIELD_COUNT, hsh_info(ftidx[i]))
	T(TEST, "wordbag metrics:");
	DO(FTI_FIELD_COUNT, hsh_info(wordbags[i]))

	//tok_pack();
	//Arr terms = arr_init(ftidx[5]->cnt), ;
	//hsh_each(ftidx[5], tok_ftidx_inspect_each, NULL);
	//hsh_each(wordbags[5], tok_wordbag_inspect_each, NULL);

	//! test ui
	C q[LINE_BUF]; USR_LOOP(usr_input_str(q, "Search query", "Inavalid characters"), tok_search(q));

	hsh_destroy(stopwords);
	DO(FTI_FIELD_COUNT, hsh_each(ftidx[2+i], tok_ftidx_destroy_each, NULL));
	DO(FTI_FIELD_COUNT, hsh_destroy(ftidx[i]));

	db_close();
	
	T(TEST, "done");
	R0;
}

//:~


