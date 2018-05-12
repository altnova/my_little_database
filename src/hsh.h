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
	sz 		vcnt; 		//< total values
	sz 		bcnt; 		//< occupied buckets
	sz 		mem;		//< total byte size
	BKT* 	buckets;	//< pointer to array of bucket pointers
} pHT;

const Z sz SZ_HT = SZ(pHT); //< hash table header size
typedef pHT* HT;


