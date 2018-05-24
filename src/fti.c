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

#define PACK_EVERY_N_RECS    (8*4096)
#define WORDBAG_INIT_SIZE    1048576
#define STATBAG_INIT_SIZE    1048576
#define FTI_STOPWORDS_FILE  "dat/stopwords.txt"
#define FTI_MIN_TOK_LENGTH   2
#define USE_WORDBAG          1

Z FTI_INFO fti_info;

Z HT   FTI[FTI_FIELD_COUNT];		//< inverted index, term -> docset

Z VEC  results[FTI_FIELD_COUNT];	//< raw matching docsets, per field
Z SET  out[FTI_FIELD_COUNT];		//< docset intersections, per field

Z HT   stopwords;	    //< stop words
Z HT   wordbag;		    //< term -> stem
Z BAG  wordbag_store;   //< stem heap
Z VEC  docmap;			//< doc_id -> rec_id
Z BAG  statbag;         //< central storage of docset stats

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
	J val = (J)bkt->payload;
	*(J*)arg += val;
	T(TEST, "%15s\t%10ld", bkt->s, val);}

V fti_print_memmap() {
	LOG("fti_mem");
	J ttl = 0;
	hsh_each(fti_info->memmap, fti_memmap_print_each, &ttl);
	T(TEST, "%15s\t%10ld", "total", ttl);
	O("\n");
	T(TEST, "%15s\t%10ld", "docset mileage", fti_info->total_docset_length);
	T(TEST, "%15s\t%10ld", "longest docset", fti_info->longest_docset);
}

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

ZI fti_find_shortest_docset(VEC docsets){
	DOCSET ds = *vec_at(docsets,0,DOCSET);
	I shortest=0, j, len, min_len=set_size(ds->docs);	
	DO(vec_size(docsets)-1,
		j=i+1; ds = *vec_at(docsets,j,DOCSET);
		len = set_size(ds->docs);
		if(len < min_len) {
			shortest = j;
			min_len = len;
		})
	R shortest;}

ZV fti_intersect(I field) {
	LOG("fti_intersect");
	VEC res = results[field];
	sz cnt = vec_size(res);
	DOCSET ds;
	//! clear previous result
	set_clear(out[field]); 
	//! bail if no docsets
	if(!cnt)R; 
	//! special case: just one docset, clone it
	if(cnt==1){
		ds = *vec_at(res,0,DOCSET);
		set_clone(out[field], ds->docs);R;}
	//! more than one docset: calculate intersection
	I shortest = fti_find_shortest_docset(res);
	ds = *vec_at(res,shortest,DOCSET);
	SET base = ds->docs;
	DO(cnt,
		if(i==shortest)continue;
		ds = *vec_at(res,i,DOCSET);
		set_intersect(base,ds->docs,out[field]))
	R 0;
}

DOCSET fti_get_docset(I field, S term, I termlen) {
	R(DOCSET)hsh_get_payload(FTI[field], term, termlen);
}

Z ID fti_docmap_translate(FTI_DOCID doc_id) {
	R *(ID*)vec_at_(docmap, doc_id);
}

V fti_search(S query, FTI_SEARCH_CALLBACK fn) {
	LOG("fti_search");
	I qlen = scnt(query);
	lcse(query,qlen); //< lowercase
	T(TRACE, "raw query: -> (%s)", query);
	//! flush docset vectors
	DO(FTI_FIELD_COUNT, vec_clear(results[i])) 
	//! tokenize and accumulate docsets per field/term
	stok(query, qlen, FTI_TOKEN_DELIM, 0,
		if(tok_len<FTI_MIN_TOK_LENGTH||hsh_get(stopwords, tok, tok_len))continue;
		//! apply stemmer
		tok_len = stm(tok, 0, tok_len-1)+1;
		tok[tok_len] = 0; //< include null
		//T(TRACE, "getting docset for %s (%d)", tok, tok_len);
		(DO(FTI_FIELD_COUNT,
			DOCSET docset = fti_get_docset(i, tok, tok_len);
			if(!docset){T(DEBUG, "%d: no docset for %s", i, tok);continue;}
			T(DEBUG, "%d: docset: %lu", i, set_size(docset->docs));
			vec_add(results[i],docset);
		)))
	//! calculate docset intersection per field and translate to rec_id
	DO(FTI_FIELD_COUNT,
		I total_hits = fti_intersect(i);
		SET s = out[i];
		sz l = set_size(s);
		if(!l) continue;
		TSTART();
		T(TEST, "%d: \e[1;37m∩(%d,%d)\e[0m -> ", i, vec_size(results[i]),total_hits);
		DO(l,
			FTI_DOCID doc_id = *vec_at(s->items, i, FTI_DOCID);
			ID rec_id = fti_docmap_translate(doc_id);
			T(TEST, "%lu ", rec_id);
		)
		TEND();
	)
	//! TODO scoring!
}

C fti_compare_docids(V*a, V*b, sz s){
	FTI_DOCID x = *(FTI_DOCID*)a;
	FTI_DOCID y = *(FTI_DOCID*)b;
	P(x==y,0)
	R x<y?-1:1;}

Z inline DOCSET fti_docset_init() {
	DOCSET d = (DOCSET)malloc(SZ_DOCSET); //! no chk() for speed
	d->docs = set_init(SZ(FTI_DOCID),(CMP)fti_compare_docids);
	d->stat = bag_init(2 * SZ(FTI_STAT_FIELD));
	R d;}

Z inline V fti_update_docset(DOCSET ds, FTI_DOCID doc_id, I doc_pos) {
	LOG("fti_update_docset");
	C exists = set_add(ds->docs, (FTI_DOCID*)&doc_id);
	//! for new element:
	if(exists) { 
		FTI_STAT_FIELD st[2]; st[0]=1; // init frequency
		st[1] = (FTI_STAT_FIELD)doc_pos; // init proximity
		bag_add(ds->stat, &st, 2 * SZ(FTI_STAT_FIELD));
		R;}
	//! for existing element:
	UJ pos = set_index_of(ds->docs, &doc_id);
	FTI_STAT_FIELD*st = ((BAG)ds->stat)->ptr + pos * (2 * SZ(FTI_STAT_FIELD));
	//T(TEST, "stat %d -> %d %d", doc_id, st[0], st[1]);
	st[0]++; // increment frequency
	st[1] = (st[1] + doc_pos) / st[0]; // update proximity avg
}

UJ fti_index_field(ID rec_id, I field, S s, I flen, FTI_DOCID doc_id) {
	LOG("fti_index_field");
	lcse(s, flen); //< lowercase
	I tok_cnt = 0;
	stok(s, flen, FTI_TOKEN_DELIM, 0,

    	if(tok_len<FTI_MIN_TOK_LENGTH||hsh_get(stopwords, tok, tok_len)){
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

	fti_info->total_records++;
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
	fti_info->total_docset_length += n;
	fti_info->longest_docset = MAX(fti_info->longest_docset, n);

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
		G*st = docset->stat->ptr + i * 2;
		T(TEST, " [%3u %3u]", (G)st[0], (G)st[1]);
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

	fti_dec_mem("wordbag_store", bag_destroy(wordbag_store));
	fti_dec_mem("statbag", bag_destroy(statbag));
	fti_dec_mem("stopwords", hsh_destroy(stopwords));
	fti_dec_mem("file_index", db_close());
	fti_dec_mem("wordbag", hsh_destroy(wordbag));

	DO(FTI_FIELD_COUNT,
		sz docsets_dealloc = 0;
		hsh_each(FTI[i], fti_terms_destroy_each, &docsets_dealloc);
		fti_dec_mem("docsets", docsets_dealloc);
		fti_dec_mem("fti", hsh_destroy(FTI[i]));
		set_destroy(out[i]);
		vec_destroy(results[i]);
	)

	fti_dec_mem("docmap", vec_destroy(docmap));

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

	// init fti
	DO(FTI_FIELD_COUNT,
		FTI[i] = hsh_init(2,1);
		results[i] = vec_init(1,SET);
		out[i] = set_init(SZ(FTI_DOCID), (CMP)fti_compare_docids);
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
	fti_inc_mem("stopwords", stopwords->mem);
	//hsh_info(stopwords);

	clock_t idx_start = clk_start();

	//! build inverted index
	UJ res = idx_each(fti_index_rec, NULL);

	X(res==NIL,T(FATAL, "unable to index records"), 1);
	T(INFO, "indexed \e[1;37m%lu records in %lums\e[0m", res, clk_diff(idx_start, clk_start()));
	T(TEST, "stopword hits %lu", fti_info->stopword_matches);

	bag_compact(wordbag_store);
	if(wordbag_store->offset)
		hsh_each(wordbag, (HT_EACH)fti_wordbag_adjust_ptr, (V*)wordbag_store->offset);

	fti_inc_mem("wordbag", wordbag->mem);
	fti_inc_mem("docmap", vec_mem(docmap));
	fti_inc_mem("wordbag_store", bag_mem(wordbag_store));

	DO(FTI_FIELD_COUNT,
		sz docsets_alloc = 0;
		hsh_each(FTI[i], fti_docsets_pack_each, &docsets_alloc);
		fti_inc_mem("docsets", docsets_alloc);
		fti_inc_mem("fti", FTI[i]->mem);
		hsh_info(FTI[i]);
	)
	bag_compact(statbag);
	fti_inc_mem("statbag", bag_mem(statbag));
	T(TEST, "packed docsets in %lums", clk_stop());

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

	//test_vec = vec_init(50000, char[11]);
	//X(!test_vec, T(FATAL, "cannot initialize test vector"), 1)

	//! test tokenizer
	S c = "a aaron abaissiez abandon aaron aaron abandoned abase abash abate abated abatement abatements abates abbess abbey abbeys abbominable abbot a";
	C qq[scnt(c)]; mcpy(qq,c,scnt(c));
	//fti_search(qq);//exit(0);

	//! test tokenizer
	//mcpy(qq,c,scnt(c));fti_index_field(0, 5, qq, 0); exit(0);

	//hsh_each(wordbag, fti_wordbag_inspect_each, NULL);
	//fti_inc_mem("test_vec", vec_mem(test_vec));
	//fti_bench();
	//fti_dec_mem("test_vec", vec_destroy(test_vec));

	fti_print_memmap();

	C qqq[16];
	mcpy(qqq,"text read includ",16);

	fti_search(qqq, NULL);

	R fti_shutdown();}

#endif

//:~


