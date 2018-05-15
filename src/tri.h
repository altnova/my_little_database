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

ext TRIE tri_init();

ext NODE tri_insert(TRIE t, S key, V*payload);
ext NODE tri_upsert(TRIE t, S key, V*payload);
ext NODE tri_get(TRIE t, S key);
ext C tri_is_leaf(NODE n);
ext V tri_dump(TRIE t, NODE curr, I depth);

typedef V(*TRIE_EACH)(NODE n, V*arg, I depth); //< tri_each function interface
ext V tri_each(TRIE t, TRIE_EACH fn, V*arg);

ext V tri_destroy(TRIE t);