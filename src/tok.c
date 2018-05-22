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
#include "vec.h"
#include "idx.h"
#include "clk.h"
#include "stm.h"
#include "usr.h"
#include "tri.h"
#include "bag.h"
#include "rnd.h"
#include "bin.h"
#include "set.h"
#include "tok.h"

#define PACK_EVERY_N_RECS    (8*4096)
#define WORDBAG_INIT_SIZE    1048576
#define FTI_STOPWORDS_FILE  "dat/stopwords.txt"
#define USE_WORDBAG          1

Z FTI_INFO fti_info;

Z HT   terms[FTI_FIELD_COUNT];

Z HT   stopwords;	    //< stop words
Z HT   wordbag;		    //< term -> stem
Z BAG  wordbag_store;   //< stem heap
Z SET  docmap;			//< doc_id -> rec_id

ZC MEM_TRACE=0;
ZV tok_inc_mem(S label, J bytes) {
	LOG("tok_inc_mem");
	if(MEM_TRACE)
		T(TEST, "\e[33m%s alloc %lu\e[0m", label, bytes);
	fti_info->total_mem += bytes;
	fti_info->total_alloc_cnt++;

	hsh_ins(fti_info->memmap, label, scnt(label), (V*)bytes);}

ZV tok_dec_mem(S label, J bytes) {
	LOG("tok_dec_mem");
	if(MEM_TRACE)	
		T(TEST, "\e[37m%s free %lu\e[0m", label, bytes);
	fti_info->total_mem -= bytes;
	fti_info->total_alloc_cnt--;}

ZV tok_memmap_print_each(BKT bkt, V*arg, HTYPE i) {
	LOG("tok_mem");
	T(TEST, "%s\t\t%lu", bkt->s, (UJ)bkt->payload);}

V tok_print_memmap() {
	hsh_each(fti_info->memmap, tok_memmap_print_each, NULL);}

Z UJ tok_load_stop_words(S fname) {
	LOG("tok_load_stop_words");
	FILE*fd;
	xfopen(fd, fname, "r+", NIL);
	UJ fsz = fsize(fd);
	S  buf = malloc(fsz); chk(buf,NIL);
	UJ bytesRead = fread(buf, 1, fsz, fd);
	S  delim = "\n";
	// tokenize
	lcse(buf, scnt(buf));
	stok(buf, fsz, "\n", 0,
		hsh_ins(stopwords, tok, tok_len, NULL);)
	// cleanup
	fclose(fd);
	free(buf);
	// optimize
	hsh_pack(stopwords);
	//hsh_dump(stopwords);
	R stopwords->cnt;}

ZI tok_cmp_qsort(const V*a, const V*b) {R ((UJ)a)-((UJ)b);}
ZV tok_wordbag_adjust_ptr(BKT bkt, V*diff, UJ i) { bkt->payload -= (J)diff; }

/*
ZV tok_wordbag_adjust_ptr(NODE bkt, V*diff, I depth) { bkt->payload -= (J)diff; }
ZV tok_store_completion(NODE n, VEC**vec, I depth) {if(n&&n->payload)vec_add(*vec, n);}
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
}*/

V tok_search(S query, FTI_SEARCH_CALLBACK fn) {
	LOG("tok_search");
	I qlen = scnt(query);
	lcse(query,qlen); //< lowercase
	T(TEST, "raw query: -> (%s)", query);
	SET prev_docset;
	stok(query, qlen, FTI_TOKEN_DELIM, 0,
		if(tok_len<2||hsh_get(stopwords, tok, tok_len))continue;
		//! apply stemmer
		tok_len = stm(tok, 0, tok_len-1)+1;
		tok[tok_len] = 0; //< include null
		T(TEST, "sending %s (%d)", tok, tok_len);

		(DO(FTI_FIELD_COUNT,
			SET docset;
			if(!docset){T(TEST, "no docset for %s", tok);continue;}
			T(TEST, "docset: %lu", set_size(docset));
		))
	)
}

UJ tok_index_field(ID rec_id, I field, S s, I flen, UJ DOC_ID) {
	LOG("tok_index_field");
	lcse(s, flen); //< lowercase
	I tok_cnt = 0;
	stok(s, flen, FTI_TOKEN_DELIM, 0,

    	if(tok_len<2||hsh_get(stopwords, tok, tok_len)){
    		fti_info->stopword_matches++;continue;}

       	#ifdef USE_WORDBAG
		BKT b = hsh_ins(wordbag, tok, tok_len, NULL);
		if(!b->payload) {
			//! apply stemmer
			tok_len = stm(tok, 0, tok_len-1)+1;
			tok[tok_len] = 0;

			//T(TEST, "STM %s -> (%d)", tok, tok_len);

			//! add stem to wordbag
			S bagtok = (S)bag_add(wordbag_store, tok, tok_len+1);

			if(wordbag_store->offset)
				hsh_each(wordbag, (HT_EACH)tok_wordbag_adjust_ptr, (V*)wordbag_store->offset);

			//! link stem to word
			b->payload = bagtok;
			fti_info->total_tokens++;
		} else {
			tok = b->payload;
			tok_len = scnt(tok);
		}
		#else
		//! apply stemmer
		tok_len = stm(tok, 0, tok_len-1)+1;
		tok[tok_len] = 0;
		#endif

		BKT term = hsh_ins(terms[field], tok, tok_len, NULL);
		if(!term->payload)
			term->payload = set_init(SZ(UH),(CMP)cmp_);

		set_add(term->payload, (UH*)&DOC_ID);
		tok_cnt++;
	)

	if(((I)DOC_ID%PACK_EVERY_N_RECS)==0) {
		clk_start();
		hsh_pack(terms[field]);
		T(DEBUG, "packed terms in %lums", clk_stop());
	}

	R tok_cnt;}

Z UJ tok_index_rec(Rec r, V*arg, UJ i) {
	tok_index_field(r->rec_id, fld_publisher, r->publisher, r->lengths[0], i);
	tok_index_field(r->rec_id, fld_title, r->title, r->lengths[1], i);
	tok_index_field(r->rec_id, fld_author, r->author, r->lengths[2], i);
	tok_index_field(r->rec_id, fld_subject, r->subject, r->lengths[3], i);
	fti_info->total_records++;
	set_add(docmap, &i); // TODO
	R0;}

ZV tok_terms_inspect_each(BKT bkt, V*arg, HTYPE i) {
	LOG("terms_each");
	SET docset = (SET)(bkt->payload);
	UJ save = vec_compact(&docset->items);
	UJ* alloc = (UJ*)arg;
	*alloc += vec_mem(docset->items);
	/*
	T(TEST, "%s (%d) -> [frame=%d used=%lu size=%lu mem=%lu lf=%.2f save=%lu] %p", bkt->s, bkt->n,
		docset->items->el_size, docset->items->used, docset->items->size, 
		vec_mem(docset->items), vec_lfactor(docset->items), save, docset);
	*/
	//TSTART();T(TEST,"(");
	//	DO(set_size(docset),T(TEST, "%lu ", *vec_at(docset->items,i,UH)));
	//	T(TEST, ") = %lu", set_size(docset));
	//TEND();
	//if(2>docset->items->used)R;
	//DO(set_size(docset), O("%lu ", *vec_at(docset->items,i,UH)));O("\n\n");
}

ZV tok_terms_destroy_each(BKT bkt, V*arg, HTYPE i) {
	SET docset = (SET)(bkt->payload);
	UJ* alloc = (UJ*)arg;
	*alloc += vec_mem(docset->items);
	vec_destroy(docset->items);}

I tok_shutdown() {
	LOG("tok_shutdown");
	I res=0;

	tok_dec_mem("wordbag_store", bag_destroy(wordbag_store));
	tok_dec_mem("stopwords", hsh_destroy(stopwords));
	tok_dec_mem("file_index", db_close());
	tok_dec_mem("wordbag", hsh_destroy(wordbag));

	DO(FTI_FIELD_COUNT,
		sz docsets_dealloc = 0;
		hsh_each(terms[i], tok_terms_destroy_each, &docsets_dealloc);
		tok_dec_mem("docsets", docsets_dealloc);
		tok_dec_mem("terms", hsh_destroy(terms[i]));		
	)

	tok_dec_mem("docmap", set_destroy(docmap));

	if (fti_info->total_mem){
		T(WARN, "unclean shutdown, \e[91mmem=%ld, cnt=%ld\e[0m", fti_info->total_mem, fti_info->total_alloc_cnt);
		tok_print_memmap();
		res=1;
	}else
		T(INFO, "shutdown complete.");

	hsh_destroy(fti_info->memmap);
	free(fti_info);

	R res;}

I tok_init() {
	LOG("tok_init");

	fti_info = (FTI_INFO)calloc(SZ_FTI_INFO,1);chk(fti_info,1);
	fti_info->memmap = hsh_init(2,1);

	// load database
	tok_inc_mem("file_index",
		db_init(DAT_FILE, IDX_FILE));

	// init terms and docmap
	DO(FTI_FIELD_COUNT, terms[i] = hsh_init(2,1);)
	docmap = set_init(SZ(UH),(CMP)cmp_); // TODO

	//! init wordbag
	wordbag = hsh_init(2,1);
	wordbag_store = bag_init(WORDBAG_INIT_SIZE);

	//! load stop words
	stopwords = hsh_init(2,10);
	UJ swcnt = tok_load_stop_words(FTI_STOPWORDS_FILE);
	X(swcnt==NIL, T(FATAL, "cannot load stopwords"), 1);
	T(INFO, "loaded %lu stopwords", stopwords->cnt);
	tok_inc_mem("stopwords", stopwords->mem);
	hsh_info(stopwords);

	clock_t idx_start = clk_start();

	//! build inverted index
	UJ res = idx_each(tok_index_rec, NULL);

	X(res==NIL,T(FATAL, "unable to index records"), 1);
	T(INFO, "indexed \e[1;37m%lu records in %lums", res, clk_diff(idx_start, clk_start()));
	T(TEST, "stopword hits %lu", fti_info->stopword_matches);

	tok_inc_mem("wordbag", wordbag->mem);
	tok_inc_mem("docmap", vec_mem(docmap->items));
	tok_inc_mem("wordbag_store", wordbag_store->size);

	DO(FTI_FIELD_COUNT,
		sz docsets_alloc = 0;
		hsh_each(terms[i], (HT_EACH)tok_terms_inspect_each, &docsets_alloc);
		tok_inc_mem("docsets", docsets_alloc);
		tok_inc_mem("terms", terms[i]->mem);
		hsh_info(terms[i]);
	)
	T(TEST, "inspected docsets in %lums", clk_stop());

	hsh_pack(wordbag);
	hsh_info(wordbag);

	R0;}

FTI_INFO tok_info() {
	R fti_info;
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

/*
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
}*/

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

	hsh_each(wordbag, tok_wordbag_inspect_each, NULL);
	tok_inc_mem("test_vec", vec_mem(test_vec));

	//tok_bench();

	tok_dec_mem("test_vec", vec_destroy(test_vec));

	tok_shutdown();

	R0;}

#endif

//:~


