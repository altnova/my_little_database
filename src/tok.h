//! \file tok.h \brief tokenizer definitions

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

ext I tok_init();

typedef V(*FTI_SEARCH_CALLBACK)(Rec);
ext V tok_search(S query, FTI_SEARCH_CALLBACK fn);

ext V tok_print_completions_for(S query);
ext V tok_print_memmap();

ext FTI_INFO tok_info();
ext V tok_bench();
ext I tok_shutdown();