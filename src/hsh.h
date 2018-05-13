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

//! create hash table
//! \param level initial level
//! \return ptr to table, NULL if error
ext HT hsh_init(I level);

//! hash table capacity
#define hsh_capacity(ht) (ht->level * 2)

//! insert str into the hash table
//! \param ht,s table, str
//! \return ptr to permanent address or NULL if error
ext S hsh_ins(HT ht, S s);

//! lookup str in the hash table
//! \param s str
//! \return ptr to str, NULL if not found
ext S hsh_get(HT ht, S s);

//! dump hash table contents
ext V hsh_dump(HT ht);

//! hash table load factor
ext E hsh_factor(HT ht);

//! total bytes
ext sz hsh_mem(HT ht);

//! release hash table
ext V hsh_destroy(HT ht);

