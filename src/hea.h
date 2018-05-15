//! \file hea.h \brief heap api

#define HEA_GROW_FACTOR 2

typedef struct heap {
	sz size;
	sz used;
	sz cnt;
	J  diff;
	V* ptr;
} pHEAP;

#define SZ_HEAP SZ(pHEAP)
typedef pHEAP* HEAP;

