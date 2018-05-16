//! \file tok.h \brief tokenizer definitions


typedef struct fti_info {
	UJ total_records;
	UJ total_tokens;
	UJ total_terms;
	J  total_mem;

} pFTI_INFO;

#define SZ_FTI_INFO SZ(pFTI_INFO)
typedef pFTI_INFO* FTI_INFO;

