//! \file fti.c \brief full-text index
//! \see https://nlp.stanford.edu/IR-book/html/htmledition/contents-1.html

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "___.h"
#include "fti.h"

#define PACK_EVERY_N_RECS    (8*4096)
#define WORDBAG_INIT_SIZE    1048576
#define STATBAG_INIT_SIZE    1048576
#define FTI_STOPWORDS_FILE  "dat/stopwords.txt"
#define USE_WORDBAG          1

Z HT   FTI[FTI_FIELD_COUNT];		//< inverted index, term -> docset
Z HT   stopwords;	    			//< stop words
Z HT   wordbag;		    			//< term -> stem
Z BAG  wordbag_store;   			//< stem heap
Z VEC  docmap;						//< doc_id -> rec_id
Z BAG  statbag;         			//< central storage of docset stats

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
	fclose(fd);
	free(buf);
	hsh_pack(stopwords);
	R stopwords->cnt;}

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

S fti_get_stopword(S w, I wlen) {
	R (S)hsh_get(stopwords, w, wlen);}

ID fti_docmap_translate(FTI_DOCID doc_id) {
	R *(ID*)vec_at_(docmap, doc_id);}

DOCSET fti_get_docset(I field, S term, I termlen) {
	R(DOCSET)hsh_get_payload(FTI[field], term, termlen);}

FTI_STAT_FIELD* fti_get_stat(DOCSET ds, UJ pos) {
	LOG("fts_get_stat");
	UJ offset = (UJ)ds->stat;
	V*bag = statbag->ptr + offset;
	FTI_STAT_FIELD*st = bag + pos * SZ(FTI_STAT_FIELD) * 2;
	R st;}

C fti_compare_docids(V*a, V*b, sz s){
	FTI_DOCID x = *(FTI_DOCID*)a;
	FTI_DOCID y = *(FTI_DOCID*)b;
	P(x==y,0)R x<y?-1:1;}

Z inline DOCSET fti_docset_init() {
	DOCSET d = (DOCSET)malloc(SZ_DOCSET); //! no chk() for speed
	d->docs = set_init(SZ(FTI_DOCID),(CMP)fti_compare_docids);
	d->stat = bag_init(2 * SZ(FTI_STAT_FIELD));
	R d;}

Z inline V fti_update_docset(DOCSET ds, FTI_DOCID doc_id, I doc_pos) {
	LOG("fti_update_docset");
	C exists = set_add(ds->docs, (FTI_DOCID*)&doc_id);
	//! new element:
	if(exists) { 
		FTI_STAT_FIELD st[2]; st[0]=1; // init frequency
		st[1] = (FTI_STAT_FIELD)doc_pos; // init proximity
		bag_add(ds->stat, &st, 2 * SZ(FTI_STAT_FIELD));
		R;}
	//! existing element:
	UJ pos = set_index_of(ds->docs, &doc_id);
	FTI_STAT_FIELD*st = ((BAG)ds->stat)->ptr + pos * (2 * SZ(FTI_STAT_FIELD));
	st[0]++; // increment frequency
	st[1] = (st[1] + doc_pos) / st[0]; // update proximity avg
}

UJ fti_index_field(ID rec_id, I field, S s, I flen, FTI_DOCID doc_id) {
	LOG("fti_index_field");
	lcse(s, flen); //< lowercase
	I tok_cnt = 0;
	stok(s, flen, FTI_TOKEN_DELIM, 0,

    	if(tok_len<FTI_MIN_TOK_LENGTH||hsh_get(stopwords, tok, tok_len)){
    		mem_info()->stopword_matches++;continue;}

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
			mem_info()->total_tokens++;
		} else {
			tok = b->payload;
			tok_len = scnt(tok);
		}
		#else
		//! apply stemmer
		tok_len = stm(tok, 0, tok_len-1)+1;
		tok[tok_len] = 0;
		#endif

		//! add term to fti
		BKT term = hsh_ins(FTI[field], tok, tok_len, NULL);
		//! init docset
		if(!term->payload)
			term->payload = fti_docset_init();
		//! update docset
		DOCSET ds = (DOCSET)term->payload;
		fti_update_docset(ds, doc_id, tok_cnt);

		tok_cnt++;
	)

	if(((I)doc_id%PACK_EVERY_N_RECS)==0) {
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

	mem_info()->total_records++;
	vec_add(docmap, r->rec_id);
	R0;}

ZV fti_docsets_pack_each(BKT bkt, V*arg, HTYPE i) {
	LOG("fti_docsets_pack_each");
	DOCSET docset = (DOCSET)(bkt->payload);
	UJ savings = set_compact(docset->docs);

	// copy dynamic statbag to permanent storage
	BAG sbg = (BAG)docset->stat;
	V*ptr = bag_add(statbag, sbg->ptr, sbg->used);
	bag_destroy(sbg); // release temp statbag
	docset->stat = (V*)(ptr - statbag->ptr); // store offset!
	//T(TEST, "%d offset %lu, stbag=%lu", i, docset->stat, bag_mem(statbag));
	
	UJ* alloc = (UJ*)arg;
	*alloc += set_mem(docset->docs);
	*alloc += SZ_DOCSET;

	sz n = set_size(docset->docs);
	mem_info()->total_docset_length += n;
	mem_info()->longest_docset = MAX(mem_info()->longest_docset, n);

	//docset->cnt = n;
	
	//T(TEST, "%s (%d) -> [frame=%d used=%lu size=%lu mem=%lu lf=%.2f bag=%lu save=%lu]", bkt->s, bkt->n,
	//	docset->docs->items->el_size, docset->docs->items->used, docset->docs->items->size, 
	//	set_mem(docset->docs), vec_lfactor(docset->docs->items), bag_mem(docset->stat), savings);
	
	/*
	TSTART();T(TEST,"%s\t->", bkt->s);
	DO(set_size(docset->docs),
		T(TEST, " %9lu", *vec_at(docset->docs->items,i,FTI_DOCID));
	);TEND();
	TSTART();T(TEST,"%s\t-> ", bkt->s);
	DO(set_size(docset->docs),
		FTI_STAT_FIELD*st = statbag->ptr + ((UJ)docset->stat) + i * SZ(FTI_STAT_FIELD) * 2;
		T(TEST, " [%3u %3u]", st[0], st[1]);
	);TEND();O("\n");
	*/

	//! catch potential stopwords
	//if(5000>docset->cnt)R;T(TEST, "%s -> %lu", bkt->s, n);
}

ZV fti_terms_destroy_each(BKT bkt, V*arg, HTYPE i) {
	DOCSET docset = (DOCSET)(bkt->payload);
	UJ* dealloc = (UJ*)arg;
	*dealloc += set_mem(docset->docs);
	*dealloc += SZ_DOCSET;
	set_destroy(docset->docs);
	free(docset);}

I fti_shutdown() {
	LOG("fti_shutdown");
	I res=0;

	mem_dec("wordbag_store", bag_destroy(wordbag_store));
	mem_dec("statbag", bag_destroy(statbag));
	mem_dec("stopwords", hsh_destroy(stopwords));
	mem_dec("file_index", db_close());
	mem_dec("wordbag", hsh_destroy(wordbag));

	DO(FTI_FIELD_COUNT,
		sz docsets_dealloc = 0;
		hsh_each(FTI[i], fti_terms_destroy_each, &docsets_dealloc);
		mem_dec("docsets", docsets_dealloc);
		mem_dec("fti", hsh_destroy(FTI[i]));
	)

	mem_dec("docmap", vec_destroy(docmap));

	R mem_shutdown();}

I fti_init() {
	LOG("fti_init");

	mem_init();

	// load database
	mem_inc("file_index",
		db_init(DAT_FILE, IDX_FILE));

	// init fti
	DO(FTI_FIELD_COUNT,
		FTI[i] = hsh_init(2,1);
	)

	docmap = vec_init(1, ID);

	//! init wordbag
	wordbag = hsh_init(2,1);
	wordbag_store = bag_init(WORDBAG_INIT_SIZE);
	statbag = bag_init(STATBAG_INIT_SIZE);

	//! load stop words
	stopwords = hsh_init(2,10);
	UJ swcnt = fti_load_stop_words(FTI_STOPWORDS_FILE);
	X(swcnt==NIL, T(FATAL, "cannot load stopwords"), 1);
	T(INFO, "loaded %lu stopwords", stopwords->cnt);
	mem_inc("stopwords", stopwords->mem);
	//hsh_info(stopwords);

	clock_t idx_start = clk_start();

	//! build inverted index
	UJ res = idx_each(fti_index_rec, NULL);

	X(res==NIL,T(FATAL, "unable to index records"), 1);
	T(INFO, "indexed \e[1;37m%lu records in %lums\e[0m", res, clk_diff(idx_start, clk_start()));
	T(TEST, "stopword hits %lu", mem_info()->stopword_matches);

	bag_compact(wordbag_store);
	if(wordbag_store->offset)
		hsh_each(wordbag, (HT_EACH)fti_wordbag_adjust_ptr, (V*)wordbag_store->offset);

	mem_inc("wordbag", wordbag->mem);
	mem_inc("docmap", vec_mem(docmap));
	mem_inc("wordbag_store", bag_mem(wordbag_store));

	DO(FTI_FIELD_COUNT,
		sz docsets_alloc = 0;
		hsh_each(FTI[i], fti_docsets_pack_each, &docsets_alloc);
		mem_inc("docsets", docsets_alloc);
		mem_inc("fti", FTI[i]->mem);
		if(FTI[i]->cnt)
			hsh_info(FTI[i]);
	)
	bag_compact(statbag);
	mem_inc("statbag", bag_mem(statbag));
	T(TEST, "packed docsets in %lums", clk_stop());

	hsh_pack(wordbag);
	hsh_info(wordbag);

	R0;}


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
	R fti_shutdown();}

#endif

//:~


