//! \file fti.c \brief full-text index

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
#include "fti.h"

typedef UH FTI_DOCID; 
#define PACK_EVERY_N_RECS    (8*4096)
#define WORDBAG_INIT_SIZE    1048576
#define FTI_STOPWORDS_FILE  "dat/stopwords.txt"
#define USE_WORDBAG          1

Z FTI_INFO fti_info;

Z HT   FTI[FTI_FIELD_COUNT];		//< inverted index, term -> docset

Z VEC  results[FTI_FIELD_COUNT];	//< raw matching docsets, per field
Z SET  out[FTI_FIELD_COUNT];		//< docset intersections, per field

Z HT   stopwords;	    //< stop words
Z HT   wordbag;		    //< term -> stem
Z BAG  wordbag_store;   //< stem heap
Z SET  docmap;			//< doc_id -> rec_id

ZC MEM_TRACE=0;
ZV fti_inc_mem(S label, J bytes) {
	LOG("fti_inc_mem");
	if(MEM_TRACE)
		T(TEST, "\e[33m%s alloc %lu\e[0m", label, bytes);
	fti_info->total_mem += bytes;
	fti_info->total_alloc_cnt++;
	BKT b = hsh_ins(fti_info->memmap, label, scnt(label), NULL);
	b->payload += bytes;
}

ZV fti_dec_mem(S label, J bytes) {
	LOG("fti_dec_mem");
	if(MEM_TRACE)	
		T(TEST, "\e[37m%s free %lu\e[0m", label, bytes);
	fti_info->total_mem -= bytes;
	BKT b = hsh_get_bkt(fti_info->memmap, label, scnt(label));
	b->payload -= bytes;
	fti_info->total_alloc_cnt--;}

ZV fti_memmap_print_each(BKT bkt, V*arg, HTYPE i) {
	LOG("fti_mem");
	T(TEST, "%s\t\t%ld", bkt->s, (J)bkt->payload);}

V fti_print_memmap() {
	hsh_each(fti_info->memmap, fti_memmap_print_each, NULL);}

Z UJ fti_load_stop_words(S fname) {
	LOG("fti_load_stop_words");
	FILE*fd;
	xfopen(fd, fname, "r+", NIL);
	UJ fsz = fsize(fd);
	S  buf = malloc(fsz); chk(buf,NIL);
	fread(buf, 1, fsz, fd);
	S  delim = "\n";
	// tokenize
	lcse(buf, fsz);
	stok(buf, fsz, "\n", 0,
		hsh_ins(stopwords, tok, tok_len, NULL);)
	// cleanup
	fclose(fd);
	free(buf);
	// optimize
	hsh_pack(stopwords);
	//hsh_dump(stopwords);
	R stopwords->cnt;}

ZI fti_cmp_qsort(const V*a, const V*b) {R ((UJ)a)-((UJ)b);}
ZV fti_wordbag_adjust_ptr(BKT bkt, V*diff, UJ i) { bkt->payload -= (J)diff; }

V fti_print_completions_for(S query) {
	LOG("fti_get_completions_for");
	T(WARN, "not yet implemented");
}
/*
ZV fti_wordbag_adjust_ptr(NODE bkt, V*diff, I depth) { bkt->payload -= (J)diff; }
ZV fti_store_completion(NODE n, VEC**vec, I depth) {if(n&&n->payload)vec_add(*vec, n);}
V fti_print_completions_for(S query) {
	LOG("fti_get_completions_for");
	VEC results = vec_init(10,NODE);
	tri_each_from(wordbag, tri_get(wordbag, query), (TRIE_EACH)fti_store_completion, &results);
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

V fti_intersect(I field) {
	VEC res = results[field];
	sz cnt = vec_size(res);
	set_clear(out[field]);
	if(!cnt)R;
	if(cnt==1){set_clone(out[field], *vec_at(res,0,SET));R;}
	out[field] = set_init(SZ(FTI_DOCID),(CMP)cmp_);
	I shortest = 0, min_len = set_size(*vec_at(res,0,SET));
	DO(cnt-1,
		SET s = *vec_at(res,i+1,SET);
		if(set_size(s) < min_len) {
			shortest = i+1;
			min_len = set_size(s);
		}
	)
	SET base = *vec_at(res,shortest,SET);
	DO(cnt,
		if(i==shortest)continue;
		SET s = *vec_at(res,i,SET);
		set_intersection(base,s,out[field]))
}

V fti_search(S query, FTI_SEARCH_CALLBACK fn) {
	LOG("fti_search");
	I qlen = scnt(query);
	lcse(query,qlen); //< lowercase
	T(TEST, "raw query: -> (%s)", query);
	VEC results[FTI_FIELD_COUNT];
	DO(FTI_FIELD_COUNT, vec_clear(results[i]))
	stok(query, qlen, FTI_TOKEN_DELIM, 0,
		if(tok_len<2||hsh_get(stopwords, tok, tok_len))continue;
		//! apply stemmer
		tok_len = stm(tok, 0, tok_len-1)+1;
		tok[tok_len] = 0; //< include null
		T(TEST, "sending %s (%d)", tok, tok_len);
		(DO(FTI_FIELD_COUNT,
			SET docset = hsh_get_payload(FTI[i], tok, tok_len);
			if(!docset){T(DEBUG, "%d: no docset for %s", i, tok);continue;}
			T(TEST, "%d: docset: %lu", i, set_size(docset));
			vec_add(results[i],docset);
		))
	)
	DO(FTI_FIELD_COUNT, fti_intersect(i))
}

UJ fti_index_field(ID rec_id, I field, S s, I flen, UJ DOC_ID) {
	LOG("fti_index_field");
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
				hsh_each(wordbag, (HT_EACH)fti_wordbag_adjust_ptr, (V*)wordbag_store->offset);

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

		BKT term = hsh_ins(FTI[field], tok, tok_len, NULL);
		if(!term->payload)
			term->payload = set_init(SZ(FTI_DOCID),(CMP)cmp_);

		set_add(term->payload, (FTI_DOCID*)&DOC_ID);
		tok_cnt++;
	)

	if(((I)DOC_ID%PACK_EVERY_N_RECS)==0) {
		clk_start();
		hsh_pack(FTI[field]);
		T(DEBUG, "packed fti in %lums", clk_stop());
	}

	R tok_cnt;}

Z UJ fti_index_rec(Rec r, V*arg, UJ i) {
	fti_index_field(r->rec_id, fld_publisher, r->publisher, r->lengths[0], i);
	fti_index_field(r->rec_id, fld_title, r->title, r->lengths[1], i);
	fti_index_field(r->rec_id, fld_author, r->author, r->lengths[2], i);
	fti_index_field(r->rec_id, fld_subject, r->subject, r->lengths[3], i);
	fti_info->total_records++;
	set_add(docmap, &i); // TODO
	R0;}

ZV fti_terms_inspect_each(BKT bkt, V*arg, HTYPE i) {
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
	//	DO(set_size(docset),T(TEST, "%lu ", *vec_at(docset->items,i,FTI_DOCID)));
	//	T(TEST, ") = %lu", set_size(docset));
	//TEND();
	//if(2>docset->items->used)R;
	//DO(set_size(docset), O("%lu ", *vec_at(docset->items,i,FTI_DOCID)));O("\n\n");
}

ZV fti_terms_destroy_each(BKT bkt, V*arg, HTYPE i) {
	SET docset = (SET)(bkt->payload);
	UJ* alloc = (UJ*)arg;
	*alloc += vec_mem(docset->items);
	vec_destroy(docset->items);}

I fti_shutdown() {
	LOG("fti_shutdown");
	I res=0;

	fti_dec_mem("wordbag_store", bag_destroy(wordbag_store));
	fti_dec_mem("stopwords", hsh_destroy(stopwords));
	fti_dec_mem("file_index", db_close());
	fti_dec_mem("wordbag", hsh_destroy(wordbag));

	DO(FTI_FIELD_COUNT,
		sz docsets_dealloc = 0;
		hsh_each(FTI[i], fti_terms_destroy_each, &docsets_dealloc);
		fti_dec_mem("docsets", docsets_dealloc);
		fti_dec_mem("fti", hsh_destroy(FTI[i]));
		set_destroy(out[i]);
		vec_destroy(results[i]); // TODO iterate and destroy sets
	)

	fti_dec_mem("docmap", set_destroy(docmap));

	if (fti_info->total_mem){
		T(WARN, "unclean shutdown, \e[91mmem=%ld, cnt=%ld\e[0m", fti_info->total_mem, fti_info->total_alloc_cnt);
		fti_print_memmap();
		res=1;
	}else
		T(INFO, "shutdown complete.");

	hsh_destroy(fti_info->memmap);
	free(fti_info);

	R res;}

I fti_init() {
	LOG("fti_init");

	fti_info = (FTI_INFO)calloc(SZ_FTI_INFO,1);chk(fti_info,1);
	fti_info->memmap = hsh_init(2,1);

	// load database
	fti_inc_mem("file_index",
		db_init(DAT_FILE, IDX_FILE));

	// init fti and docmap
	DO(FTI_FIELD_COUNT,
		FTI[i] = hsh_init(2,1);
		results[i] = vec_init(1,SET);
		out[i] = set_init(SZ(FTI_DOCID), (CMP)cmp_);
	)

	docmap = set_init(SZ(FTI_DOCID),(CMP)cmp_); // TODO

	//! init wordbag
	wordbag = hsh_init(2,1);
	wordbag_store = bag_init(WORDBAG_INIT_SIZE);

	//! load stop words
	stopwords = hsh_init(2,10);
	UJ swcnt = fti_load_stop_words(FTI_STOPWORDS_FILE);
	X(swcnt==NIL, T(FATAL, "cannot load stopwords"), 1);
	T(INFO, "loaded %lu stopwords", stopwords->cnt);
	fti_inc_mem("stopwords", stopwords->mem);
	hsh_info(stopwords);

	clock_t idx_start = clk_start();

	//! build inverted index
	UJ res = idx_each(fti_index_rec, NULL);

	X(res==NIL,T(FATAL, "unable to index records"), 1);
	T(INFO, "indexed \e[1;37m%lu records in %lums", res, clk_diff(idx_start, clk_start()));
	T(TEST, "stopword hits %lu", fti_info->stopword_matches);

	fti_inc_mem("wordbag", wordbag->mem);
	fti_inc_mem("docmap", set_mem(docmap));
	fti_inc_mem("wordbag_store", bag_mem(wordbag_store));

	DO(FTI_FIELD_COUNT,
		sz docsets_alloc = 0;
		hsh_each(FTI[i], (HT_EACH)fti_terms_inspect_each, &docsets_alloc);
		fti_inc_mem("docsets", docsets_alloc);
		fti_inc_mem("fti", FTI[i]->mem);
		hsh_info(FTI[i]);
	)
	T(TEST, "inspected docsets in %lums", clk_stop());

	hsh_pack(wordbag);
	hsh_info(wordbag);

	R0;}

FTI_INFO fti_stats() {
	R fti_info;
}

#ifdef RUN_TESTS_FTI

VEC test_vec;

ZV fti_wordbag_inspect_each(BKT bkt, V*arg, HTYPE i) {
	LOG("fti_wordbag_inspect_each");
	if(bkt->n<11 && mcmp(bkt->s, "malachy", 7)) {
		//T(TEST, "test_vec add: %s", bkt->s);
		vec_add_((V**)&test_vec, bkt->s);
	}
}

/*
V fti_bench() {
	LOG("fti_bench");
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
	LOG("fti_test");
	srand(time(NULL));

	P(fti_init(),1);

	test_vec = vec_init(50000, char[11]);
	X(!test_vec, T(FATAL, "cannot initialize test vector"), 1)

	//! test tokenizer
	S c = "a aaron abaissiez abandon aaron aaron abandoned abase abash abate abated abatement abatements abates abbess abbey abbeys abbominable abbot a";
	C qq[scnt(c)]; mcpy(qq,c,scnt(c));
	//fti_search(qq);//exit(0);
	
	//! test tokenizer
	//mcpy(qq,c,scnt(c));fti_index_field(0, 5, qq, 0); exit(0);

	hsh_each(wordbag, fti_wordbag_inspect_each, NULL);
	fti_inc_mem("test_vec", vec_mem(test_vec));

	//fti_bench();

	fti_dec_mem("test_vec", vec_destroy(test_vec));

	R fti_shutdown();}

#endif

//:~


