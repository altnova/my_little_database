//! \file set.c \brief ordered set api


typedef struct set {
	VEC items;
	CMP cmpfn;
} pSET;

#define SZ_SET SZ(pSET)
typedef pSET* SET;

ext SET set_init(sz el_size, CMP cmpfn);
ext SET set_clone(SET s, SET from);

ext V*  set_get(SET s, V*key);
ext V*  set_get_at(SET s, sz i);
ext C   set_add(SET s, V*key);
ext sz  set_add_all(SET s, V*keys, sz n);
ext C   set_contains(SET s, SET subset);
ext sz  set_size(SET s);
ext sz  set_mem(SET s);
ext V   set_clear(SET s);
ext V   set_intersection(SET a, SET b, SET dest);

ext sz  set_compact(SET s); // shrink underlying vector
ext sz  set_destroy(SET s);

//:~
