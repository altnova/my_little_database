//!\file hsh.h \brief hash table api

#pragma once

#include "vec.h"

typedef UI HTYPE; //< hash width
typedef HTYPE(*HSH_FN)(S val, UI len); //< hsh_each function interface

typedef struct bucket{
	HTYPE h;				//< value hash
	UH n;					//< value len
	struct bucket* next;	//< pointer to next 
//	HTYPE idx;				//< table index
	C packed;				//< in heap
	V* payload;				//< pointer to payload
	G s[];					//< value bytes
} __attribute__((packed))  pBKT;

const Z sz SZ_BKT = SZ(pBKT); //< bucket header size
typedef pBKT* BKT;

typedef struct hash_table {
	HTYPE	split;		//< split position
	HTYPE	level;		//< capacity is 2^level
	H		rounds;		//< split rounds	
	HTYPE	cnt; 		//< total values
	HTYPE	bcnt;		//< occupied buckets
	HSH_FN  fn;			//< hash function
	sz 		mem;		//< total byte size
	V*		heap;		//< bucket heap pointer
	BKT* 	buckets;	//< pointer to array of bucket pointers
} pHT;

const Z sz SZ_HT = SZ(pHT); //< hash table header size
typedef pHT* HT;

//! create hash table
//! \param level initial level, must be a power of two
//! \param srounds split rounds, higher is faster lookups but slower inserts
//! \param fn hash function
//! \return ptr to table, NULL if error
ext HT hsh_init(I level, H split_rounds);
ext HT hsh_init_custom(I level, H srounds, HSH_FN fn);

//! passthrough hash function
ext HTYPE hsh_identity(V*a,UJ n);

//! hash table capacity
#define hsh_capacity(ht) (ht->level * 2)

//! insert str into the hash table
//! \param ht,s,n tab,key,len \param payload pointer to payload struct
//! \return ptr to permanent address or NULL if error
ext BKT hsh_ins(HT ht, V*k, sz n, V*payload);

//! lookup key,payload,bucket by key
//! \param s,n key,len
//! \return ptr to str, NULL if not found
ext V*  hsh_get(HT ht, V*k, sz n);
ext V*  hsh_get_payload(HT ht, V*k, sz n);
ext BKT hsh_get_bkt(HT ht, V*k, sz n);

//! dump hash table metrics
ext V hsh_info(HT ht);

//! dump hash table contents
ext V hsh_dump(HT ht);

//! \brief average bucket tail length
//! better performance when closer to 1
//! \see HT->rounds
ext E  hsh_bavg(HT ht);

//! \brief load factor
//! better keyspace utilization when closer to 1
ext E  hsh_factor(HT ht);

//! total bytes
ext sz hsh_mem(HT ht);

//! pack values into a contiguous heap to improve data locality
//! \return 1 if ok, 0 if error
ext C  hsh_pack(HT ht);

//! apply fn() to each bucket in the table
//! \param fn function that takes (BT bkt, V* arg, HTYPE i)
//! \param arg argument to be passed to each fn() call
typedef V(*HT_EACH)(BKT bkt, V*arg, HTYPE i); //< hsh_each function interface
ext V  hsh_each(HT ht, HT_EACH fn, V*arg);

//! print bucket contents
ext V  hsh_print(BKT b);

//! release memory
ext sz hsh_destroy(HT ht);


//:~


