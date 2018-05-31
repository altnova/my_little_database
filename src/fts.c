//! \file fts.c \brief full-text search

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include "___.h"
#include "fts.h"

Z VEC  results[FTI_FIELD_COUNT];	//< raw matching docsets, per field
Z FTI_MATCH mbuf;					//< buffer
Z VEC HITS;							//< final assembly of the search result

ZV fts_dump_docset(S label, SET s) {
	LOG("fts_dbg");
	TSTART();T(TEST, "%s -> (%2d) ", label, set_size(s));
	DO(set_size(s),
		T(TEST, "%d ", *(FTI_DOCID*)set_at(s,i));
	)TEND();}

V fts_dump_result() {
	LOG("fts_dump_result");
	TSTART();T(TEST, "result (%d) -> ", vec_size(HITS));
	DO(vec_size(HITS),
		FTI_MATCH m = vec_at_(HITS,i);
		T(TEST, "%lu(%.3f) ", m->rec_id, m->score);
	)TEND();}

ZI fts_find_shortest_docset(VEC docsets){
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

ZE fts_get_score(FTI_DOCID doc_id, DOCSET ds, UJ pos) {
	LOG("fts_get_score");
	FTI_STAT_FIELD*st = fti_get_stat(ds,pos);
	F tf = sqrt((F)st[0]);
	F idf = 1 + log((F)mem_info()->total_records/(1+set_size(ds->docs)));
	idf *= idf; // square it
	//T(TEST, "recovered stat: id=%d freq=%d dfreq=%d prox=%d", doc_id, st[0], set_size(ds->docs), st[1]);
	//T(TEST, "score: id=%d tf=%0.4f idf=%0.4f", tf, idf);
	R tf * idf;}

//! comparator for qsort
ZI fts_compare_matches(const V*a, const V*b) {
	LOG("fts_compare");
	FTI_MATCH x = (FTI_MATCH)a;
	FTI_MATCH y = (FTI_MATCH)b;
	//T(TEST, "qsort %lu %lu", x->rec_id, x->rec_id);
	P(x->score==y->score, 0)
	C r = x->score > y->score; //< ascending
	R r?-1:1;}

ZV fts_sort_matches() {
	LOG("fts_sort");
	T(TRACE, "sorting matches...");
	qsort(HITS->data, vec_size(HITS), SZ_FTI_MATCH, fts_compare_matches);}

Z FTI_MATCH fts_add_match(FTI_DOCID doc_id, I field, F score) {
		ID rec_id = fti_docmap_translate(doc_id);
		mbuf->rec_id = rec_id;
		mbuf->field = 1<<field;
		mbuf->score = score;
		vec_add_((V**)&HITS, mbuf);	
		R(FTI_MATCH)vec_last_(HITS);}

ZV fts_intersect(I field) {
	LOG("fts_intersect");
	VEC res = results[field];
	sz cnt = vec_size(res);
	DOCSET ds,nxt;

	if(!cnt)R; //< bail if no docsets
	if(cnt==1){ //! just one docset, proceed to scoring
		ds = *vec_at(res,0,DOCSET);
		DO(set_size(ds->docs),
			FTI_DOCID doc_id = *(FTI_DOCID*)set_at(ds->docs,i);
			F score = fts_get_score(doc_id, ds, i);
			FTI_MATCH m = fts_add_match(doc_id, field, score))		
		R;}

	//! more than one docset: calculate intersection
	I shortest = fts_find_shortest_docset(res);
	ds = *vec_at(res,shortest,DOCSET);
	SET base = ds->docs; C match;
	//fti_dump_docset("base", base);
	C TRACE_NWAY = 0;
	if(TRACE_NWAY){TSTART();T(TEST, "%d-way matches for field %d:", cnt, field);}
	DO(set_size(base),
		FTI_DOCID needle = *(FTI_DOCID*)set_at(base, i);
		match = 1;
		F score = fts_get_score(needle, ds, i);
		DO(cnt,
			if(i==shortest)continue;	
			nxt = *vec_at(res,i,DOCSET);
			UJ idx = set_index_of(nxt->docs, &needle);
			if(idx==NIL){match=0;break;}
			score += fts_get_score(needle, nxt, idx);
		)
		if(!match)continue;

		FTI_MATCH m = fts_add_match(needle, field, score);

		if(TRACE_NWAY)T(TEST, " %d", m->rec_id);
	)
	if(TRACE_NWAY)TEND();
}

sz fts_search(S query, UI max_hits, V*arg, FTS_CALLBACK fn) {
	LOG("fts_search");
	I qlen = scnt(query);
	lcse(query,qlen); //< lowercase
	T(TEST, "raw query: -> (%s)", query);
	//! flush old docset vectors
	DO(FTI_FIELD_COUNT, vec_clear(results[i])) 
	vec_clear(HITS);
	//! tokenize and accumulate docsets per field/term
	TSTART();T(TEST, "docsets:");
	stok(query, qlen, FTI_TOKEN_DELIM, 0,
		if(tok_len<FTI_MIN_TOK_LENGTH||fti_get_stopword(tok, tok_len))continue;
		//! apply stemmer
		tok_len = stm(tok, 0, tok_len-1)+1;
		tok[tok_len] = 0; //< terminate
		//! retrieve docsets per field
		T(TEST, " %s ->", tok);
		(DO(FTI_FIELD_COUNT,
			DOCSET docset = fti_get_docset(i, tok, tok_len);
			if(!docset){T(DEBUG, "%d: no docset for %s", i, tok);continue;}
			T(TEST, " (%d:%lu)", i, set_size(docset->docs));
			vec_add(results[i],docset);
		)))TEND();

	//! calculate docset intersection, poopulate and sort HITS
	DO(FTI_FIELD_COUNT, fts_intersect(i))
	fts_sort_matches();
	//fts_dump_result();
	R vec_size(HITS);
}

I fts_shutdown() {
	LOG("fts_shutdown");
	I res=0;

	DO(FTI_FIELD_COUNT,
		vec_destroy(results[i]);
	)

	vec_destroy(HITS);
	free(mbuf);

	R fti_shutdown();}


I fts_init() {
	LOG("fts_init");

	P(fti_init(),1);

	DO(FTI_FIELD_COUNT,
		results[i] = vec_init(1,DOCSET);
	)

	HITS = vec_init_(1,SZ_FTI_MATCH);
	mbuf = (FTI_MATCH)calloc(SZ_FTI_MATCH,1);

	R0;
}

#ifdef RUN_TESTS_FTS

I main() {
	LOG("fts_test");

	srand(time(NULL));
	P(fts_init(),1);

	//S Q = "text read includ";
	S Q = "tolstoy";
	C qqq[scnt(Q)+1];
	mcpy(qqq,Q,scnt(Q)+1);

	clk_start();
	I hits = fts_search(qqq, NULL);
	T(TEST, "found %d documents in %lums", hits, clk_stop());	

	fts_dump_result();

	R fts_shutdown();}

#endif




//:~




