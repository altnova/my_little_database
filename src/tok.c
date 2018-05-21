//! \file tok.c \brief string tokenizer

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
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
#include "rnd.h"
#include "bin.h"
#include "set.h"
#include "tok.h"

#define PACK_EVERY_N_RECS (4096)
#define WORDBAG_INIT_SIZE 1048576

Z FTI_INFO fti_info;

Z HT terms;
Z HT stopwords;			//< hash table: stop words
Z HT wordbag_ht;
Z TRIE wordbag; 		//< trie: global term -> stem
Z BAG wordbag_store;



ZC MEM_TRACE=0;
ZV tok_inc_mem(S label, J bytes) {
	LOG("tok_inc_mem");
	if(MEM_TRACE)
		T(TEST, "\e[33m%s alloc %lu\e[0m", label, bytes);
	fti_info->total_mem += bytes;
	fti_info->total_alloc_cnt++;

	hsh_ins(fti_info->memmap, label, scnt(label), (V*)bytes);
}

ZV tok_dec_mem(S label, J bytes) {
	LOG("tok_dec_mem");
	if(MEM_TRACE)	
		T(TEST, "\e[37m%s free %lu\e[0m", label, bytes);
	fti_info->total_mem -= bytes;
	fti_info->total_alloc_cnt--;
}

Z UJ tok_load_stop_words(S fname) {
	LOG("tok_load_stop_words");
	FILE*fd;
	xfopen(fd, fname, "r+", NIL);
	UJ fsz = fsize(fd);
	S buf = malloc(fsz); chk(buf,NIL);
	UJ bytesRead = fread(buf, 1, fsz, fd);
	lcse(buf, scnt(buf));
	S delim = "\n";

	stok(buf, fsz, "\n", 0,
		hsh_ins(stopwords, tok, tok_len, NULL);
	)

	fclose(fd);
	free(buf);

	hsh_pack(stopwords);
	hsh_info(stopwords);
	//hsh_dump(stopwords);
	R stopwords->cnt;
}

FTI_INFO tok_info() {
	R fti_info;
}

//! comparator for qsort()
ZI tok_cmp_qsort(const V*a, const V*b) {
	R ((UJ)a)-((UJ)b);
}

ZV tok_wordbag_adjust_ptr(NODE bkt, V*diff, I depth) { bkt->payload -= (J)diff; }
ZV tok_wordbag_ht_adjust_ptr(BKT bkt, V*diff, UJ i) { bkt->payload -= (J)diff; }

ZV tok_store_completion(NODE n, VEC**vec, I depth) {
	LOG("tok_store_completion");
	if(n&&n->payload)
		vec_add(*vec, n);
}

V tok_print_completions_for(S query) {
	LOG("tok_get_completions_for");
	VEC results = vec_init(10,NODE);
	tri_each_from(wordbag, tri_get(wordbag, query), (TRIE_EACH)tok_store_completion, &results);
	O("%s:", query);
	I rlen = scnt(query);
	if(!vec_size(results))
		O("\e[91m%s\e[0m", "no completions");
	DO(vec_size(results),
		NODE n = *vec_at(results, i, NODE);
		I len = n->depth - rlen;
		C cpl[len+1];
		DO(len, cpl[len-i-1]=n->key; n=n->parent;) //< bubble up to root
		cpl[len]=0; //< terminate
		if(len)
			O(" %s\e[33m%s\e[0m", query, cpl)
	)
	vec_destroy(results);
}

V tok_search(S query) {
	LOG("tok_search");
	I qlen = scnt(query);
	lcse(query,qlen); //< lowercase
	//T(TEST, "raw query: -> (%s)", query);
	TSTART();
	T(TEST, "stemmed search terms: ");
	stok(query, qlen, FTI_TOKEN_DELIM, 0,
		if((tok_len<2||hsh_get(stopwords, tok, tok_len)))continue;
		//! apply stemmer
		tok_len = stm(tok, 0, tok_len-1)+1;
		tok[tok_len] = 0; //< include null
		T(TEST, "(%s) ", tok);
	)
	TEND();
}

ZC USE_WORDBAG = 1;
const ZI STOK_TRACE=0, STOK_VAL_LEN=5; ZS STOK_VAL = "aaron";
UJ tok_index_field(ID rec_id, I field, S s, I flen, UJ DOC_ID) {
	LOG("tok_index_field");
	lcse(s, flen); //< lowercase
	I tok_cnt = 0;
	stok(s, flen, FTI_TOKEN_DELIM, 0,

    	if(tok_len<2||hsh_get(stopwords, tok, tok_len)){
    		fti_info->stopword_matches++;continue;}

		//if(STOK_TRACE&&!mcmp(tok,STOK_VAL, STOK_VAL_LEN))
		//	T(TEST, "TOK %s -> (%s %d)", STOK_VAL, tok, tok_len);
       	C from_wordbag = 0;
       	if(USE_WORDBAG) {

			//NODE b =
			//tri_insert(wordbag, tok, tok_len, 1);
			
			BKT b = hsh_ins(wordbag_ht, tok, tok_len, NULL);

			if (!b){
				T(WARN, "skipping bad token at rec=%lu fld=%s pos=%d", rec_id, rec_field_names[field], tok_pos);
				continue; //< unsupported characters
			}

			if(!b->payload) {
				//! apply stemmer
				tok_len = stm(tok, 0, tok_len-1)+1;
				tok[tok_len] = 0;

				if(STOK_TRACE&&!mcmp(tok,STOK_VAL,STOK_VAL_LEN))
					T(TEST, "STM %s -> (%d)", tok, tok_len);

				//! add stem to wordbag
				S bagtok = (S)bag_add(wordbag_store, tok, tok_len+1);

				//if(wordbag_store->offset)
				//	tri_each(wordbag, tok_wordbag_adjust_ptr, (V*)wordbag_store->offset);

				if(wordbag_store->offset)
					hsh_each(wordbag_ht, (HT_EACH)tok_wordbag_ht_adjust_ptr, (V*)wordbag_store->offset);
				//! link stem to word
				b->payload = bagtok;

				fti_info->total_tokens++;
			} else {
				tok = b->payload;
				tok_len = scnt(tok);
				from_wordbag = 1;
				//term = hsh_get(terms, tok, scnt(tok)+1);
			}
		} else {
			//! apply stemmer
			tok_len = stm(tok, 0, tok_len-1)+1;
			tok[tok_len] = 0;
		}

		BKT term = hsh_ins(terms, tok, tok_len, NULL);
		//T(TEST, "term => %p %s", term, term->s);
		if(!term->payload)
			term->payload = set_init(SZ(UI), (CMP)cmp_);

		set_add(term->payload, (UI*)&DOC_ID);

		tok_cnt++;
	)

	if(((I)DOC_ID%PACK_EVERY_N_RECS)==0) {
		clk_start();
		hsh_pack(terms);
		T(DEBUG, "packed terms in %lums", clk_stop());
	}

	R tok_cnt;
}

Z UJ tok_index_rec(Rec r, V*arg, UJ i) {
	tok_index_field(r->rec_id, fld_publisher, r->publisher, r->lengths[0], i);
	tok_index_field(r->rec_id, fld_title, r->title, r->lengths[1], i);
	tok_index_field(r->rec_id, fld_author, r->author, r->lengths[2], i);
	tok_index_field(r->rec_id, fld_subject, r->subject, r->lengths[3], i);
	fti_info->total_records++;
	R0;
}

ZV tok_memmap_print_each(BKT bkt, V*arg, HTYPE i) {
	LOG("tok_mem");
	T(TEST, "%s\t\t%lu", bkt->s, (UJ)bkt->payload);
}

V tok_print_memmap() {
	hsh_each(fti_info->memmap, tok_memmap_print_each, NULL);
}

ZV tok_terms_inspect_each(BKT bkt, V*arg, HTYPE i) {
	LOG("terms_each");
	SET docset = (SET)(bkt->payload);
	UJ save = vec_compact(&docset->items);
	UJ* alloc = (UJ*)arg;
	*alloc += vec_mem(docset->items);
	T(TRACE, "%s -> [used=%lu size=%lu mem=%lu lf=%.2f save=%lu]", bkt->s,
		docset->items->used, docset->items->size, vec_mem(docset->items), vec_lfactor(docset->items), save);
}

ZV tok_terms_destroy_each(BKT bkt, V*arg, HTYPE i) {
	SET docset = (SET)(bkt->payload);
	UJ* alloc = (UJ*)arg;
	*alloc += vec_mem(docset->items);
	vec_destroy(docset->items);
}

ZV tok_pack() {
	LOG("tok_pack");
	clk_start();
	T(TEST, "packed indexes in %lums", clk_stop());
}

ZV tok_ftidx_destroy_each(BKT bkt, V*arg, HTYPE i) {
	VEC a = (VEC)bkt->payload;
	sz*released = (sz*)arg;
	*released = (*released)+vec_destroy(a);
}

ZV tok_ftidx_inspect_each(BKT bkt, V*arg, HTYPE i) {
	LOG("tok_ftidx_inspect_each");
	VEC wordbag = (VEC)bkt->payload;
	//TSTART();
	//if(vec_size(wordbag)>1000) {
	if(STOK_TRACE&&!mcmp(bkt->s, "aaro",4))
		T(TEST, "(%s) %lu %d", bkt->s, vec_size(wordbag), bkt->h);
}

I tok_shutdown() {
	LOG("tok_shutdown");
	I res=0;

	tok_dec_mem("wordbag", tri_destroy(wordbag));
	tok_dec_mem("wordbag_store", bag_destroy(wordbag_store));
	tok_dec_mem("stopwords", hsh_destroy(stopwords));

	tok_dec_mem("file_index", db_close());
	tok_dec_mem("wordbag_ht", hsh_destroy(wordbag_ht));

	UJ docvectors_dealloc = 0;
	hsh_each(terms, tok_terms_destroy_each, &docvectors_dealloc);
	tok_dec_mem("docvectors", docvectors_dealloc);
	tok_dec_mem("terms", hsh_destroy(terms));

	if (fti_info->total_mem){
		T(INFO, "shutdown incomplete, \e[91mmem=%ld, cnt=%ld\e[0m", fti_info->total_mem, fti_info->total_alloc_cnt);
		res=1;
	}else
		T(INFO, "shutdown complete.");

	hsh_destroy(fti_info->memmap);
	free(fti_info);

	R res;
}

I tok_init() {
	LOG("tok_init");

	fti_info = (FTI_INFO)calloc(SZ_FTI_INFO,1);chk(fti_info,1);
	fti_info->memmap = hsh_init(2,1);

	tok_inc_mem("file_index",
		db_init(DAT_FILE, IDX_FILE));

	terms = hsh_init(2,1);

	//! init wordbag
	wordbag = tri_init();
	wordbag_store = bag_init(WORDBAG_INIT_SIZE);
	wordbag_ht = hsh_init(2,1);

	//! load stop words
	stopwords = hsh_init(2,3);
	UJ swcnt = tok_load_stop_words("dat/stopwords.txt");
	X(swcnt==NIL, T(FATAL, "cannot load stopwords"), 1);
	T(INFO, "loaded %lu stopwords", stopwords->cnt);
	tok_inc_mem("stopwords", stopwords->mem);

	clock_t idx_start = clk_start();

	//! build inverted index
	UJ res = idx_each(tok_index_rec, NULL);

	X(res==NIL,T(FATAL, "unable to index records"), 1);
	T(TEST, "indexed %lu records in %lums", res, clk_diff(idx_start, clk_start()));
	T(TEST, "stopword hits %lu", fti_info->stopword_matches);

	tok_inc_mem("wordbag", wordbag->mem);
	tok_inc_mem("wordbag_store", wordbag_store->size);
	tok_inc_mem("wordbag_ht", wordbag_ht->mem);
	tok_inc_mem("terms", terms->mem);

	hsh_pack(terms);
	T(TEST, "packed terms in %lums", clk_stop());

	sz docvectors_alloc = 0;
	hsh_each(terms, (HT_EACH)tok_terms_inspect_each, &docvectors_alloc);
	tok_inc_mem("docvectors", docvectors_alloc);
	T(TEST, "inspected docvectors in %lums", clk_stop());

	hsh_pack(wordbag_ht);
	hsh_info(wordbag_ht);

	//hsh_dump(terms);
	hsh_info(terms);

	R0;
}

#ifdef RUN_TESTS_TOK

VEC test_vec;

ZV tok_wordbag_inspect_each(BKT bkt, V*arg, HTYPE i) {
	LOG("tok_wordbag_inspect_each");
	if(bkt->n<11 && mcmp(bkt->s, "malachy", 7)) {
		//T(TEST, "test_vec add: %s", bkt->s);
		vec_add_((V**)&test_vec, bkt->s);
	}
}

V tok_bench() {
	LOG("tok_bench");
	clk_start();
	sz ROUNDS = 10000000;
	I HITS = 0;
	
	T(TEST, "trie=%lu (%lu) ht=%d (%lu)", wordbag->cnt, wordbag->mem, wordbag_ht->cnt, wordbag_ht->mem);

	NODE n;
	S needle;
	C buf[11];
	DO(ROUNDS,
		needle = vec_random(test_vec);
		if(0&&(rand()%10)==5){
			rnd_str(buf,10,CHARSET_az);
			n = tri_get(wordbag, buf);
			if(n)HITS++;//else O("%s\n", buf);
		} else {
			n = tri_get(wordbag, needle);
			if(n)HITS++;//else O("%s\n", needle);
		}
	)
	T(TEST, "trie rounds: %d hits in %lums", HITS, clk_stop());

	//DO(rounds,stm(str, 0, len-1);//O("%s\n", str);)T(TEST, "stem rounds: %lums", clk_stop());

	BKT b;
	HITS = 0;
	DO(ROUNDS,
		needle = vec_random(test_vec);
		if(0&&(rand()%10)==5){
			rnd_str(buf,10,CHARSET_az);
			b = hsh_get_bkt(wordbag_ht, buf, 11);
			if(b)HITS++;//else O("%s\n", buf);
		} else {
			b = hsh_get_bkt(wordbag_ht, needle, scnt(needle));
			if(b)HITS++;//else O("%s\n", needle);
		}
		//O("%s\n", b->s);
	)
	T(TEST, "htab rounds: %d hits in %lums", HITS, clk_stop());	
}

I main() {
	LOG("tok_test");
	srand(time(NULL));

	P(tok_init(),1);

	test_vec = vec_init(50000, char[11]);
	X(!test_vec, T(FATAL, "cannot initialize test vector"), 1)

	//! test tokenizer
	S c = "a aaron abaissiez abandon aaron aaron abandoned abase abash abate abated abatement abatements abates abbess abbey abbeys abbominable abbot a";
	C qq[scnt(c)]; mcpy(qq,c,scnt(c));
	//tok_search(qq);//exit(0);
	
	//! test tokenizer
	//mcpy(qq,c,scnt(c));tok_index_field(0, 5, qq, 0); exit(0);

	hsh_each(wordbag_ht, tok_wordbag_inspect_each, NULL);
	tok_inc_mem("test_vec", vec_mem(test_vec));

	//tok_bench();

	tok_dec_mem("test_vec", vec_destroy(test_vec));
	tok_shutdown();
	
	R0;
}

#endif

//:~


