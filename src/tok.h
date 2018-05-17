//! \file tok.h \brief tokenizer definitions


typedef struct fti_info {
	UJ total_records;
	UJ total_tokens;
	UJ total_terms;
	J  total_mem;

} pFTI_INFO;

#define SZ_FTI_INFO SZ(pFTI_INFO)
typedef pFTI_INFO* FTI_INFO;

ext I tok_init();
ext V tok_search(S query);
ext V tok_print_completions_for(S query);

ext FTI_INFO tok_info();
ext V tok_bench();
ext I tok_shutdown();