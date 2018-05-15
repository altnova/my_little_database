//! \file tri.h \brief trie api

#define TRI_RANGE 26
#define TRI_RANGE_OFFSET 97

typedef struct trie {
	UJ cnt;
	struct node* root;
} pTRIE;

#define SZ_TRIE SZ(pTRIE)
typedef pTRIE* TRIE;

typedef struct node {
	UJ id;
	C key;
	V* payload;
	struct node* children[TRI_RANGE];
} pNODE;

#define SZ_NODE SZ(pNODE)
typedef pNODE* NODE;