//! \file fti.h \brief full-text index interface

#include "adt/set.h"
#include "adt/hsh.h"
#include "adt/bag.h"
#include "fio.h"
#include "stm.h"
#include "clk.h"
#include "mem.h"
#include "idx.h"

#define FTI_MIN_TOK_LENGTH   2

typedef UH FTI_DOCID;
typedef  G FTI_STAT_FIELD;

typedef struct docset {
	SET docs; //< doc_ids
	V*  stat; //< frequencies and proximity averages
	//UH  cnt;
} pDOCSET;
#define SZ_DOCSET SZ(pDOCSET)
typedef pDOCSET* DOCSET;

ext I fti_init();

ext FTI_STAT_FIELD* fti_get_stat(DOCSET ds, UJ pos);
ext S fti_get_stopword(S w, I wlen);
ext DOCSET fti_get_docset(I field, S term, I termlen);
ext ID fti_docmap_translate(FTI_DOCID doc_id);
ext V fti_print_completions_for(S query);

//ext V fti_bench();
ext I fti_shutdown();

//:~

