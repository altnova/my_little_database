//! \file mem.h \brief memory controller api

#pragma once

#include "adt/hsh.h"

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

ext I mem_init();
ext I mem_shutdown();

ext FTI_INFO mem_info();
ext DB_INFO mem_db_info();
ext V mem_inc(S label, J bytes);
ext V mem_dec(S label, J bytes);
ext V mem_map_print();

//:~

