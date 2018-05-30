//! \file mem.c \brief memory controller

#include <stdlib.h>
#include <string.h>
#include "___.h"
#include "mem.h"

Z FTI_INFO fti_info;
Z DB_INFO db_info;

ZC MEM_TRACE=0;
V mem_inc(S label, J bytes) {
	LOG("mem_inc");
	if(MEM_TRACE)
		T(TEST, "\e[33m%s alloc %lu\e[0m", label, bytes);
	fti_info->total_mem += bytes;
	fti_info->total_alloc_cnt++;
	BKT b = hsh_ins(fti_info->memmap, label, scnt(label), NULL);
	b->payload += bytes;}
 
DB_INFO mem_db_info() {
	db_info->total_records=fti_info->total_records;
	db_info->total_words=fti_info->total_tokens;
	db_info->total_mem=fti_info->total_mem;
	R db_info;}

V mem_dec(S label, J bytes) {
	LOG("mem_dec");
	if(MEM_TRACE)	
		T(TEST, "\e[37m%s free %lu\e[0m", label, bytes);
	fti_info->total_mem -= bytes;
	BKT b = hsh_get_bkt(fti_info->memmap, label, scnt(label));
	b->payload -= bytes;
	fti_info->total_alloc_cnt--;}

ZV mem_map_print_each(BKT bkt, V*arg, HTYPE i) {
	LOG("mem_map");
	J val = (J)bkt->payload;
	*(J*)arg += val;
	T(TEST, "%15s\t%10ld", bkt->s, val);}

V mem_map_print() {
	LOG("mem_map");
	J ttl = 0;
	hsh_each(fti_info->memmap, mem_map_print_each, &ttl);
	T(TEST, "%15s\t%10ld", "total", ttl);
	O("\n");
	T(TEST, "%15s\t%10ld", "docset mileage", fti_info->total_docset_length);
	T(TEST, "%15s\t%10ld", "longest docset", fti_info->longest_docset);}

I mem_init() {
	LOG("mem_init");

	fti_info = (FTI_INFO)calloc(SZ_FTI_INFO,1);chk(fti_info,1);
	fti_info->memmap = hsh_init(2,1);

	db_info = (DB_INFO)calloc(SZ_DB_INFO,1);chk(db_info,1);
	R0;
}

FTI_INFO mem_info() {
	R fti_info;}

I mem_shutdown() {
	LOG("mem_shutdown");

	if (fti_info->total_mem){
		T(WARN, "unclean shutdown, \e[91mmem=%ld, cnt=%ld\e[0m", fti_info->total_mem, fti_info->total_alloc_cnt);
		mem_map_print();
		R1;
	}else
		T(INFO, "shutdown complete.");

	hsh_destroy(fti_info->memmap);
	free(fti_info);
	free(db_info);

	R0;
}


//:~

