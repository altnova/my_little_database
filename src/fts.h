//! \file fts.c \brief full-text search api

#pragma once

//#include "adt/set.h"
#include "fti.h"

typedef struct fti_match {
	 G field;
	 F score;
  pRec r;
} pFTI_MATCH;
#define SZ_FTI_MATCH SZ(pFTI_MATCH)
typedef pFTI_MATCH* FTI_MATCH;

ext I fts_init();

typedef V(*FTS_CALLBACK)(FTI_MATCH m, V*arg, UI i);
ext sz fts_search(S query, UI max_hits, V*arg, FTS_CALLBACK fn);

ext V fts_dump_result();
ext I fts_shutdown();


//:~