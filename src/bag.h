//! \file bag.h \brief bag api

#define BAG_GROW_FACTOR 2

typedef struct bag {
	sz size;
	sz used;
	sz cnt;
	J  offset;
	V* ptr;
} pBAG;

#define SZ_BAG SZ(pBAG)
typedef pBAG* BAG;

ext BAG  bag_init(sz init_sz);
ext V*   bag_add(BAG h, V*obj, sz obj_sz);
ext sz   bag_destroy(BAG h);

//:~