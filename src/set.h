//! \file set.c \brief set api


typedef struct set {
	UJ cnt;
	HT table;
	BAG bag;
} pSET;

#define SZ_SET SZ(pSET)
typedef pSET* SET;

//:~
