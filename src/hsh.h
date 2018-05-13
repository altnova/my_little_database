#include "___.h"

typedef struct bucket{
	I h,n;					//< hash and length of payload
	struct bucket* next;	//< pointer to next 
	sz idx;					//< current ht index
	C s[];					//< payload
} pBKT;

const Z sz SZ_BKT = SZ(pBKT); //< bucket header size
typedef pBKT* BKT;

typedef struct hash_table {
	I 		split;		//< 
	I 		level;		//< 
	sz 		cnt; 		//< total values
	sz 		mem;		//< total byte size
	BKT* 	buckets;	//< pointer to array of bucket pointers
} pHT;

const Z sz SZ_HT = SZ(pHT); //< hash table header size
typedef pHT* HT;

//! \brief hash table capacity
#define hsh_capacity(ht) (ht->level * 2)

//! \brief lookup str in the hash table \param s str
//! \return ptr to str, NULL if not found
S hsh_get(HT ht, S s);


//! hash table load factor
E hsh_factor(HT ht);

//! total bytes occupied by hash table
ext sz hsh_mem(HT ht);


