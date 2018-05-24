//! \file fti.h \brief full-text index interface

typedef struct fti_info {
	UJ   total_records;
	UJ   total_tokens;
	UJ   total_terms;
	J    total_mem;
	J    total_alloc_cnt;
	UJ   stopword_matches;
	HT   memmap;
} pFTI_INFO;

#define SZ_FTI_INFO SZ(pFTI_INFO)
typedef pFTI_INFO* FTI_INFO;

typedef UH FTI_DOCID; 
typedef struct docmap_pair {
	FTI_DOCID doc_id;
	ID rec_id;
} pDOCMAP_PAIR;

#define SZ_DOCMAP_PAIR SZ(pDOCMAP_PAIR)
typedef pDOCMAP_PAIR* DOCMAP_PAIR;

ext I fti_init();

typedef V(*FTI_SEARCH_CALLBACK)(Rec);
ext V fti_search(S query, FTI_SEARCH_CALLBACK fn);

ext V fti_print_completions_for(S query);
ext SET fti_get_docset(I field, S term, I len);
ext V fti_print_memmap();

ext FTI_INFO fti_stats();
ext V fti_bench();
ext I fti_shutdown();

//:~

