//! \file fts.c \brief full-text search api

typedef struct fti_match {
	ID rec_id;
	 G field;
	 F score;
} pFTI_MATCH;
#define SZ_FTI_MATCH SZ(pFTI_MATCH)
typedef pFTI_MATCH* FTI_MATCH;

ext I fts_init();
ext I fts_shutdown();

typedef V(*FTI_SEARCH_CALLBACK)(Rec);
ext I fts_search(S query, FTI_SEARCH_CALLBACK fn);


//:~