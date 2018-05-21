//! \file set.c \brief ordered set api


typedef struct set {
	VEC items;
	CMP cmpfn;
} pSET;

#define SZ_SET SZ(pSET)
typedef pSET* SET;

//:~
