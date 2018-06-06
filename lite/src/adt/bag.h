//! \file bag.h \brief bag api

#pragma once

#define BAG_GROW_FACTOR 2

typedef struct bag {
	sz size;
	sz used;
	J  offset;
	V* ptr;
} __attribute__((packed)) pBAG;

#define SZ_BAG SZ(pBAG)
typedef pBAG* BAG;

ext BAG  bag_init(sz init_sz);
ext V*   bag_add(BAG h, V*obj, sz obj_sz);
ext sz   bag_mem(BAG h);
ext sz   bag_compact(BAG h);
ext V    bag_clear(BAG h);
ext V*   bag_data(BAG h);
ext sz   bag_destroy(BAG h);

//:~