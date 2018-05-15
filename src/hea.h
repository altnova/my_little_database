//! \file hea.h \brief heap api

#define HEA_GROW_FACTOR 2

typedef struct heap {
	sz size;
	sz used;
	sz cnt;
	J  offset;
	V* ptr;
} pHEAP;

#define SZ_HEAP SZ(pHEAP)
typedef pHEAP* HEAP;

ext HEAP hea_init(sz init_sz);
ext V*   hea_add(HEAP h, V*obj, sz obj_sz);
ext V    hea_destroy(HEAP h);
