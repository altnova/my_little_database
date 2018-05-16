//! \file tok.c \brief string tokenizer

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include "___.h"
#include "cfg.h"
#include "trc.h"
#include "fio.h"
#include "hsh.h"
#include "idx.h"
#include "clk.h"
#include "vec.h"
#include "stm.h"
#include "usr.h"
#include "tri.h"
#include "bag.h"
#include "tok.h"

#define PACK_EVERY_N_RECS (4*4096)
#define WORDBAG_INIT_SIZE 1048576

Z FTI_INFO fti_info;

Z HT stopwords;	//< hash table: stop words
Z HT ftidx[FTI_FIELD_COUNT]; //< hash tables: stem -> Vec(rec_ids)
Z HT wordbags[FTI_FIELD_COUNT]; //< hash tables: term -> stem

Z TRIE wordbag; //< trie: global term -> stem
Z BAG wordbag_store;

//Z inline V tok_lcase(S tk) {DO(scnt(tk), tk[i]=tolower(tk[i]));}

ZC MEM_TRACE=0;
V tok_inc_mem(S label, J bytes) {
	LOG("tok_inc_mem");
	if(MEM_TRACE)
		T(TEST, "\e[33m%s alloc %lu\e[m", label, bytes);
	fti_info->total_mem += bytes;}

V tok_dec_mem(S label, J bytes) {
	LOG("tok_dec_mem");
	if(MEM_TRACE)	
		T(TEST, "%s free %lu", label, bytes);
	fti_info->total_mem -= bytes;}

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
		hsh_ins(stopwords, tok, tok_len, NULL))
	fclose(fd);
	free(buf);

	hsh_pack(stopwords);
	hsh_info(stopwords);
	//hsh_dump(stopwords);

	R stopwords->cnt;
}

//! comparator for qsort()
ZI tok_cmp_qsort(const V*a, const V*b) {
	R ((UJ)a)-((UJ)b);
}

//! sort index by rec_id
ZV tok_ftidx_sort_each(BKT bkt, V*arg, UJ i) {
	LOG("tok_ftidx_sort_each");
	Vec a = (Vec)bkt->payload;
	sz*allocated = (sz*)arg;
	*allocated = (*allocated)+a->mem;
	if(a->used<2)R;
	qsort(a->data, a->used, SZ(ID), tok_cmp_qsort);
	//T(TEST, "%lu %s index sorted %lu items", i, bkt->s, a->used);
}

V tok_wordbag_adjust_ptr(NODE bkt, V*diff, I depth) { bkt->payload -= (J)diff; }

V tok_store_completion(NODE n, V*vec, I depth) {
	if(n&&n->payload)
		vec_add(vec, n);
}
V tok_print_completions_for(S radix) {
	LOG("tok_get_completions_for");
	Vec results = vec_init(10,NODE);
	tri_each_from(wordbag, tri_get(wordbag, radix), tok_store_completion, results);
	O("%s:", radix);
	I rlen = scnt(radix);
	if(!vec_size(results))
		O("\e[91m%s\e[0m", " no completions");
	DO(vec_size(results),
		NODE n = *vec_at(results, i, NODE);
		I len = n->depth - rlen;
		C cpl[len+1];
		DO(len, cpl[len-i-1]=n->key; n=n->parent;) //< bubble up to root
		cpl[len]=0; //< terminate
		if(len)
			O(" -\e[33m%s\e[0m", cpl))
	O("\n");
	vec_destroy(results);
}

V tok_search_help() {
	O("\n\tindexed fields:                             ");
	DO(FTI_FIELD_COUNT, O(" %s", rec_field_names[i])); O("\n");
	O("\n\ttotal records in index:                      %lu\n", fti_info->total_records);
	O("\ttotal distinct tokens:                       %lu\n", fti_info->total_tokens);
	O("\ttotal search terms:                          %lu\n",  fti_info->total_terms);
	O("\ttotal memory used:                           %lu\n",  fti_info->total_mem);
	O("\n");
	O("\tcombination of terms for fuzzy search:       war peace tolstoy\n");
	O("\tadd quotes to search for exact matches:      \"War and Peace\"\n");
	O("\tappend or prepend * for substring search:    dostoev*\n");
	O("\tprepend field name to search in field:       title:algernon\n");
	O("\tappend ? to display completions:             music?\n");
	O("\n");

}

V tok_search(S query) {
	I qlen = scnt(query);
	if(qlen==1&&query[0]=='?') {
		tok_search_help();
		R;
	}
	if(query[qlen-1]=='?') {
		query[qlen-1]=0;
		tok_print_completions_for(query);
		R;
	}

	LOG("tok_search");
	lcse(query); //< lowercase
	//T(TEST, "raw query: -> (%s)", query);
	TSTART();
	T(TEST, "stemmed search terms: ");
	stok(query, qlen, FTI_TOKEN_DELIM,
		if((tok_len<2||hsh_get(stopwords, tok, tok_len)))continue;
		//! apply stemmer
		tok_len = stm(tok, 0, tok_len-1)+1;
		tok[tok_len] = 0; //< include null
		T(TEST, "(%s) ", tok);
	)
	TEND();
}

ZC DO_WORDBAG = 0;
const ZI STOK_TRACE=0, STOK_VAL_LEN=5; ZS STOK_VAL = "aaron";
UJ tok_index_field(ID rec_id, I field, S s, UJ i) {
	LOG("tok_index_field");
	lcse(s); //< lowercase
	I tok_cnt = 0;
	stok(s, scnt(s), FTI_TOKEN_DELIM,

    	if((tok_len<2||hsh_get(stopwords, tok, tok_len)))continue;

		//if(STOK_TRACE&&!mcmp(tok,STOK_VAL, STOK_VAL_LEN))
		//	T(TEST, "TOK %s -> (%s %d)", STOK_VAL, tok, tok_len);
       	C from_wordbag = 0;

		NODE b;
       	if (DO_WORDBAG) {
			b = tri_insert(wordbag, tok, NULL);

			if (!b){
				T(WARN, "skipping bad token at rec=%lu fld=%s pos=%d", rec_id, rec_field_names[field], tok_pos);
				continue; //< unsupported characters
			}

			if(DO_WORDBAG&&!b->payload) {
				//! apply stemmer
				tok_len = stm(tok, 0, tok_len-1)+1;
				tok[tok_len] = 0;

				if(STOK_TRACE&&!mcmp(tok,STOK_VAL,STOK_VAL_LEN))
					T(TEST, "STM %s -> (%d)", tok, tok_len);

				//! add stem to wordbag
				S bagtok = (S)bag_add(wordbag_store, tok, tok_len+1);

				if(wordbag_store->offset){
					tri_each(wordbag, tok_wordbag_adjust_ptr, (V*)wordbag_store->offset);
		    	}
				//! link stem to word
				b->payload = bagtok;
				fti_info->total_tokens++;
			} else {
				tok = b->payload;
				from_wordbag = 1;
			}
		} else {
			//! apply stemmer
			tok_len = stm(tok, 0, tok_len-1)+1;
			tok[tok_len] = 0;
		}

		if(STOK_TRACE&&!mcmp(tok,STOK_VAL,STOK_VAL_LEN))
			T(TEST, "INS %s -> (%s %d) from_bag=%d", STOK_VAL, tok, tok_len, from_wordbag);

		tok[tok_len+1]=0;

    	BKT fti = hsh_ins(ftidx[field], tok, tok_len+1, 0);
		if(!fti->payload) fti->payload = vec_init(10, ID);
		vec_add(fti->payload, rec_id);
		tok_cnt++;
	)

	if(((I)i%PACK_EVERY_N_RECS)==0) {
		clk_start();
		hsh_pack(ftidx[field]);
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

	fti_info->total_records++;
	R0;
}

V tok_ftidx_repack_each(BKT bkt, V*arg, HTYPE i) {
	HT wordbag = (HT)(bkt->payload);
	hsh_pack(wordbag);
	//hsh_info(wordbag);
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
	Vec a = (Vec)bkt->payload;
	sz*released = (sz*)arg;
	*released = (*released)+vec_destroy(a);
}

V tok_wordbag_inspect_each(BKT bkt, V*arg, HTYPE i) {
	LOG("tok_wordbag_inspect_each");
	UJ count = *bkt->s;
	T(TEST, "%s (%d=%s)", bkt->s, count, bkt->payload);
}

V tok_ftidx_inspect_each(BKT bkt, V*arg, HTYPE i) {
	LOG("tok_ftidx_inspect_each");
	Vec wordbag = (Vec)bkt->payload;
	//TSTART();
	//if(vec_size(wordbag)>1000) {
	if(STOK_TRACE&&!mcmp(bkt->s, "aaro",4))
		T(TEST, "(%s) %lu %d", bkt->s, vec_size(wordbag), bkt->h);
}

V tok_shutdown() {
	LOG("tok_shutdown");
	tok_dec_mem("wordbag", tri_destroy(wordbag));
	tok_dec_mem("wordbag_store", bag_destroy(wordbag_store));
	tok_dec_mem("stopwords", hsh_destroy(stopwords));

	
	DO(FTI_FIELD_COUNT,
		sz released = 0;
		hsh_each(ftidx[i], tok_ftidx_destroy_each, &released);
		tok_dec_mem("vectors", released);
	)

	DO(FTI_FIELD_COUNT,
		tok_dec_mem("fti", hsh_destroy(ftidx[i]))
	)

	db_close();

	tok_dec_mem("fti_info", SZ_FTI_INFO);
	if (fti_info->total_mem)
		T(INFO, "shutdown incomplete, \e[91mmem=%ld\e[0m", fti_info->total_mem);	
	free(fti_info);
	

	O("\n");
	
}

V tok_interrupt_handler(I itr) {
    tok_shutdown();
    exit(0);
}

I main() {
	LOG("tok_test");

	signal(SIGINT, tok_interrupt_handler);

	fti_info = (FTI_INFO)calloc(SZ_FTI_INFO,1);chk(fti_info,1);
	tok_inc_mem("fti_info", SZ_FTI_INFO);

	//! test tokenizer
	S c = "a aaron abaissiez abandon aaron aaron abandoned abase abash abate abated abatement abatements abates abbess abbey abbeys abbominable abbot a";
	C qq[scnt(c)]; mcpy(qq,c,scnt(c));
	//tok_search(qq);//exit(0);
	
	db_init(DAT_FILE, IDX_FILE);

	//! init hash tables
	DO(FTI_FIELD_COUNT,
		ftidx[i]=hsh_init(2,3);
		//wordbag=hsh_init(2,3);
		X(!ftidx[i], T(FATAL, "cannot initialize hash table %d", i), 1);
	)

	//! init wordbag
	wordbag = tri_init();
	wordbag_store = bag_init(WORDBAG_INIT_SIZE);
	tok_inc_mem("wordbag_store", wordbag_store->size);

	//! load stop words
	stopwords = hsh_init(2,3);
	UJ swcnt = tok_load_stop_words("dat/stopwords.txt");
	X(swcnt==NIL,T(FATAL, "cannot load stopwords"), 1);
	T(TEST, "loaded %lu stopwords", stopwords->cnt);
	tok_inc_mem("stopwords", stopwords->mem);

	//! test tokenizer
	//mcpy(qq,c,scnt(c));tok_index_field(0, 5, qq, 0); exit(0);

	//! build inverted index
	clock_t idx_start = clk_start();
	UJ res = idx_each(tok_index_rec, NULL, 1);
	X(res==NIL,T(FATAL, "unable to index records"), 1);
	T(TEST, "indexed %lu records in %lums", res, clk_diff(idx_start, clk_start()));
	tok_inc_mem("wordbag", wordbag->mem);

	//! sort document buckets
	
	DO(FTI_FIELD_COUNT,
		sz allocated = 0;
		hsh_each(ftidx[i], (HT_EACH)tok_ftidx_sort_each, &allocated);
		tok_inc_mem("vectors", allocated);
	)
	T(TEST, "sorted buckets in %lums", clk_stop());

	T(TEST, "fti metrics:");
	DO(FTI_FIELD_COUNT, 
		tok_inc_mem("fti", ftidx[i]->mem);
		hsh_info(ftidx[i]))

	//tok_pack();
	//Vec terms = vec_init(ftidx[5]->cnt), ;
	hsh_each(ftidx[5], tok_ftidx_inspect_each, NULL);
	//hsh_each(wordbags[5], tok_wordbag_inspect_each, NULL);

	//tri_dump(wordbag);
	//tri_dump_from(wordbag, tri_get(wordbag, "music"));

	//! test ui
	O("empty line to quit, ? for help\n");
	C q[LINE_BUF]; USR_LOOP(usr_input_str(q, "Search query", "Inavalid characters"), tok_search(q));

	tok_shutdown();
	
	R0;
}

//:~


