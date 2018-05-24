//! \file fti.h \brief full-text index interface

typedef struct fti_info {
	UJ   total_records;
	UJ   total_tokens;
	UJ   total_terms;
	UJ   total_docset_length;
	sz   longest_docset;
	J    total_mem;
	J    total_alloc_cnt;
	UJ   stopword_matches;
	HT   memmap;
} pFTI_INFO;

#define SZ_FTI_INFO SZ(pFTI_INFO)
typedef pFTI_INFO* FTI_INFO;

typedef UH FTI_DOCID;
typedef G  FTI_STAT_FIELD;

typedef struct docset {
	SET docs; //< doc_ids
	V*  stat; //< frequencies and proximity averages
	//UH  cnt;
} pDOCSET;

#define SZ_DOCSET SZ(pDOCSET)
typedef pDOCSET* DOCSET;

ext I fti_init();

typedef V(*FTI_SEARCH_CALLBACK)(Rec);
ext V fti_search(S query, FTI_SEARCH_CALLBACK fn);

ext V fti_print_completions_for(S query);
ext DOCSET fti_get_docset(I field, S term, I termlen);
ext V fti_print_memmap();

ext FTI_INFO fti_stats();
ext V fti_bench();
ext I fti_shutdown();

//:~

