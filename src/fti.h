//! \file fti.h \brief full-text index interface

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

ext DOCSET fti_get_docset(I field, S term, I termlen);
ext ID fti_docmap_translate(FTI_DOCID doc_id);
ext V fti_print_completions_for(S query);

//ext V fti_bench();
ext I fti_shutdown();

//:~

