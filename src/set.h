//! \file set.c \brief ordered set api


typedef struct set {
	VEC items;
	CMP cmpfn;
} pSET;

#define SZ_SET SZ(pSET)
typedef pSET* SET;

ext SET set_init(sz el_size, CMP cmpfn);
ext V*  set_get(SET s, V*key);
ext V*  set_add(SET s, V*key);
ext UJ  set_size(SET s);
ext V   set_intersection(SET a, SET b, SET dest);

ext sz   set_destroy(SET s);


//:~
