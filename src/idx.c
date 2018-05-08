#include <stdlib.h>
#include <string.h>
#include "___.h"
#include "cfg.h"
#include "arr.h"
#include "bin.h"
#include "idx.h"
#include "rec.h"
#include "trc.h"
#include "fio.h"

Z Arr *idx; //< in-memory instance of the index
Z bufRec buf; //< readbuffer RECBUFLEN records

C db_file[MAX_FNAME_LEN+1];
C idx_file[MAX_FNAME_LEN+1];

//! current idx size
UJ idx_size() {
	R idx->used;
}

//! returns specific index entry
Pair* idx_get_entry(UJ idx_pos) {
	R arr_at(idx, idx_pos, Pair);
}

//! returns pointer to indexes' data section
Pair* idx_data() {
	R (Pair*)idx->data;
}
//! create db file if not exists \returns number of records
Z UJ db_touch(S fname) {
	FILE*f = fopen(fname, "a");
	UJ sz = fsize(f)/SZ_REC;
	fclose(f);
	R sz;
}

//! comparator kernel
Z J _c(const V*a, const V*b) {
	UJ x = ((Pair*)a)->rec_id;
	UJ y = ((Pair*)b)->rec_id;
	R x-y;
}

//! comparator for binfn()
C cmp_binsearch(V*a, V*b, size_t t) {
	J r = _c(a,b);
	T(TRACE, "cmp_binsearch: r=%ld\n", r);
	R !r?r:r<0?-1:1;
}

//! comparator for qsort()
Z I cmp_qsort(const V*a, const V*b) {
	J r = _c(a,b);
	T(TRACE, "cmp_qsort %ld\n", r);
	R r;
}

//! sort index by rec_id
Z V idx_sort() {
	qsort(idx->data, idx->used, SZ(Pair), cmp_qsort);
	T(DEBUG, "idx_sort: index sorted\n");
}

//! dump index to stdout
Z V idx_dump(UJ head) {
	Pair *e;
	T(TEST, "\nidx_dump: { last_id=%lu, used=%lu } =>\n\t", idx->hdr, idx->used);
	DO(head?head:idx->used,
		e = arr_at(idx, i, Pair);
		T(TEST, " (%lu->%lu)", e->rec_id, e->pos);
	)
	T(INFO, "\n\n");
}

//! rebuild index from scratch
V idx_rebuild() {
	FILE *in = fopen(db_file, "r");
	UJ rcnt, pos=0;

	while((rcnt = fread(buf, SZ_REC, RECBUFLEN, in))) {
		T(DEBUG, "idx_rebuild: read %lu records\n", rcnt);
		Pair e; //< index entry
		DO(rcnt,
			Rec b = &buf[i];
			e.rec_id = b->rec_id;
			e.pos = pos++;
			arr_add(idx, e);
		)
	}

	fclose(in);
	idx_sort();

	Pair *last = arr_last(idx, Pair);
	idx->hdr = last->rec_id; 	//< use Arr header field for last_id

	T(INFO, "idx_rebuild: rebuilt, entries=%lu, last_id=%lu\n", idx->used, idx->hdr);
}

//! free memory
V idx_close() {
	arr_free(idx);
}

//! zap index entry at pos
UJ idx_shift(UJ pos) {
	Pair *s = arr_at(idx, pos, Pair);
	UJ to_move = idx->used-pos-1;
	memcpy(s, s+1, SZ(Pair)*to_move);
	T(DEBUG, "idx_shift: shifted %lu entries, squashed db_pos=%lu\n", to_move, pos);
	idx->used--;
	idx_save();
	R idx->used;
} 

//! persist index in a file
V idx_save() {
	FILE *out = fopen(idx_file, "w+");
	fwrite(idx, SZ(Arr)+idx->size*SZ(Pair), 1, out);
	J size = fsize(out);
	fclose(out);
	T(TRACE, "idx_save: %lu bytes\n", size);
}

//! load index from a file
V idx_load() {
	FILE *in = fopen(idx_file, "r");
	idx_close();
	J size = fsize(in);
	Arr**tmp = &idx; //< replace pointer
	*tmp = arr_init((size-SZ(Arr))/SZ(Pair), Pair);
	fread(idx, size, 1, in);
	fclose(in);
	T(INFO, "idx_load: %lu bytes, %lu entries, capacity=%lu, last_id=%lu\n", size, idx->used, idx->size, idx->hdr);
}

//! update index header on disk
V idx_update_hdr() {
	FILE *out = fopen(idx_file, "r+");
	zseek(out, 0L, SEEK_SET);
	fwrite(idx, SZ(Arr), 1, out);
	fclose(out);
	T(DEBUG, "idx_update_hdr: idx header updated\n");
}

//! get next available id and store it on disk
UJ next_id() {
	UJ id = ++idx->hdr;
	idx_update_hdr();
	return id;
}

//! patch record's pos pointer and store it on disk
UJ idx_update_pos(UJ rec_id, UJ new_pos) {
	UJ idx_pos = rec_get_idx_pos(idx, rec_id);
	BAIL_IF(idx_pos, NONE); //< no such record
	Pair *i = arr_at(idx, idx_pos, Pair);
	i->pos = new_pos;
	FILE *out = fopen(idx_file, "r+");
	zseek(out, SZ(Arr)+SZ(Pair)*(idx_pos-1), SEEK_SET);
	fwrite(i, SZ(Pair), 1, out);
	fclose(out);
	T(DEBUG, "idx_update_pos: { rec_id=%lu, new_pos=%lu }\n", rec_id, new_pos);
	R new_pos;
}

//! add new index element and save
V idx_add(UJ rec_id, UJ pos) {
	Pair e;
	e.rec_id = rec_id;
	e.pos = pos;
	arr_add(idx, e);
	T(DEBUG, "idx_add: { rec_id=%lu, pos=%lu }\n", rec_id, pos);
	idx_save(); //< TODO append single item instead of full rewrite
}

//! perform sample index lookup
Z UJ idx_peek(UJ rec_id){
	Rec b;
	UJ pos = rec_get(b, rec_id);
	BAIL_IF(pos, NONE); //< no such record
	T(TRACE, "idx_peek rec_id=%lu pos=%ld\n", rec_id, pos);
	rec_print_dbg(b);
	R pos;
}

//! dump db to stdout
Z V db_dump() {
	FILE *in = fopen(db_file, "r");
	UJ rcnt;
	while((rcnt = fread(buf, SZ_REC, RECBUFLEN, in))) {
		T(DEBUG, "db_dump: read %lu records\n", rcnt);
		DO(rcnt, rec_print_dbg(&buf[i]);)
	}
	fclose(in);
}

//! create or rebuild index if necessary
Z V idx_touch() {
	UJ db_size = db_touch(db_file);
	FILE*f = fopen(idx_file, "w+");
	UJ idx_fsize = fsize(f);
	idx = arr_init(RECBUFLEN, Pair);
	if (!idx_fsize) {
		idx_save();
		idx_load();
		T(INFO, "idx_touch: initialized empty index file\n");
	}
	if (idx->used != db_size) { //< out of sync
		idx_rebuild();
		idx_save();
		T(INFO, "idx_touch: synchronized index file\n");
	}
	fclose(f);
}

V db_init(S d, S i) {
	scpy_s(db_file, d, MAX_FNAME_LEN);
	scpy_s(idx_file, i, MAX_FNAME_LEN);
	idx_touch();
	T(INFO, "db_init: database initialized\n");
}

I test() {
	Rec b = malloc(SZ_REC);

	db_init("dat/books.dat", "dat/books.idx");

	DO(3, next_id())

	UJ delete_test = 10;
	if(rec_delete(delete_test) == NONE)
		T(WARN, "no such record %lu\n", delete_test);

	rec_get(b, 5); //< load from disk
	T(TEST, "\nbefore update: "); rec_print_dbg(b);
	H pages = 666;
	rec_set(b, fld_pages, &pages);
	rec_set(b, fld_title, "WINNIE THE POOH");
	rec_update(b);
	rec_get(b, 5); //< reload from disk
	T(TEST, "after update: "); rec_print_dbg(b); T(TEST, "\n");

	//db_dump(); idx_dump(0);
	DO(3, rec_create(b);)
	rec_delete(17);
	db_dump(); idx_dump(0);

	//rec_delete(20); //< delete last
	//db_dump(); idx_dump(0);

	idx_close();

	free(b);

	R 0;
}

I main() { R test(); }

//:~