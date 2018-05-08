#include <stdlib.h>
#include <string.h>
#include "books.h"
#include "dynarray.h"
#include "binsearch.h"
#include "idx.h"
#include "rec.h"
#include "trc.h"
#include "io.h"

Arr *book_index = 0;
C db_file[MAX_FNAME_LEN+1];
C idx_file[MAX_FNAME_LEN+1];

//! create db file if not exists \returns number of records
Z UJ db_touch(S fname) {
	FILE*f = fopen(fname, "a");
	UJ sz = fsize(f)/SZ(Book);
	fclose(f);
	R sz;
}

//! index entry comparator kernel
Z J _c(const V*a, const V*b) {
	UJ x = ((Idx*)a)->book_id;
	UJ y = ((Idx*)b)->book_id;
	R (x-y);
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

//! sort index by book_id
Z V idx_sort() {
	qsort(book_index->data, book_index->used, SZ(Idx), cmp_qsort);
	T(DEBUG, "idx_sort: index sorted\n");
}

//! rebuild index from scratch
V idx_rebuild() {
	FILE *in = fopen(db_file, "r");

	UJ rcnt, pos;
	Book buf[BUFSIZE];

	while((rcnt = fread(buf, SZ(Book), BUFSIZE, in))) {
		T(DEBUG, "idx_rebuild: read %lu records\n", rcnt);
		Idx entry;
		DO(rcnt,
			Book *b = &buf[i];
			entry.book_id = b->book_id;
			entry.pos = pos++;
			arr_add(book_index, entry);
		)
	}

	fclose(in);
	idx_sort();

	Idx *last = arr_last(book_index, Idx);
	book_index->hdr = last->book_id; 	//< use Arr header field for last_id

	T(INFO, "idx_rebuild: rebuilt, entries=%lu, last_id=%lu\n", book_index->used, book_index->hdr);
}

//! free memory
V idx_close() {
	arr_free(book_index);
}

//! zap index entry at pos
UJ idx_shift(UJ pos) {
	Idx*s = arr_at(book_index, pos, Idx);
	UJ to_move = book_index->used-pos-1;
	memcpy(s, s+1, SZ(Idx)*to_move);
	T(DEBUG, "idx_shift: shifted %lu entries\n", to_move);
	book_index->used--;
	idx_save();
	R book_index->used;
} 

//! persist index in a file
V idx_save() {
	FILE *out = fopen(idx_file, "w+");
	fwrite(book_index, SZ(Arr)+book_index->size*SZ(Idx), 1, out);
	J size = fsize(out);
	fclose(out);
	T(TRACE, "idx_save: %lu bytes\n", size);
}

//! load index from a file
V idx_load() {
	FILE *in = fopen(idx_file, "r");
	idx_close();
	J size = fsize(in);
	Arr**idx = &book_index;
	*idx = arr_init((size-SZ(Arr))/SZ(Idx), Idx);
	fread(book_index, size, 1, in);
	fclose(in);
	T(INFO, "idx_load: %lu bytes, %lu entries, capacity=%lu, last_id=%lu\n", size, book_index->used, book_index->size, book_index->hdr);
}

//! update index header on disk
V idx_update_hdr() {
	FILE *out = fopen(idx_file, "r+");
	zseek(out, 0L, SEEK_SET);
	fwrite(book_index, SZ(Arr), 1, out);
	fclose(out);
	T(DEBUG, "idx_update_hdr: idx header updated\n");
}

//! get next available id and store it on disk
UJ next_id() {
	UJ id = ++book_index->hdr;
	idx_update_hdr();
	return id;
}

//! patch record's pos pointer and store it on disk
UJ idx_update_pos(UJ book_id, UJ new_pos) {
	UJ idx_pos = rec_get_idx_pos(book_index, book_id);
	if(idx_pos==NONE)R NONE; //< no such book
	Idx *i = arr_at(book_index, idx_pos, Idx);
	i->pos = new_pos;
	FILE *out = fopen(idx_file, "r+");
	zseek(out, SZ(Arr)+SZ(Idx)*(idx_pos-1), SEEK_SET);
	fwrite(i, SZ(Idx), 1, out);
	fclose(out);
	T(DEBUG, "idx_update_pos: { book_id=%lu, new_pos=%lu }\n", book_id, new_pos);
	R new_pos;
}

V idx_add(UJ book_id, UJ pos) {
	Idx e;
	e.book_id = book_id;
	e.pos = pos;
	arr_add(book_index, e);
	T(DEBUG, "idx_add: { book_id=%lu, pos=%lu }\n", book_id, pos);
	idx_save();
}

//! perform sample index lookup
Z UJ idx_peek(UJ book_id){
	Book b;
	UJ pos = rec_get(&b, book_id);
	if (pos==NONE)R NONE;
	T(TRACE, "idx_peek book_id=%lu pos=%ld\n", book_id, pos);
	rec_print_dbg(&b);
	R pos;
}

//! dump index to stdout
Z V idx_dump(UJ head) {
	Idx *e;
	T(TEST, "\nidx_dump: { last_id=%lu, used=%lu } =>\n\t", book_index->hdr, book_index->used);
	DO(head?head:book_index->used,
		e = arr_at(book_index, i, Idx);
		T(TEST, " (%lu->%lu)", e->book_id, e->pos);
	)
	T(INFO, "\n\n");
}

//! dump db to stdout
Z V db_dump() {
	FILE *in = fopen(db_file, "r");
	Book buf[BUFSIZE];
	UJ rcnt;
	while((rcnt = fread(buf, SZ(Book), BUFSIZE, in)))
		DO(rcnt, rec_print_dbg(&buf[i]);)
	fclose(in);
}

//! create or rebuild index if necessary
Z V idx_touch() {
	UJ db_size = db_touch(db_file);
	FILE*f = fopen(idx_file, "w+");
	UJ idx_fsize = fsize(f);
	book_index = arr_init(BUFSIZE,Idx);
	if (!idx_fsize) {
		idx_save();
		idx_load();
		T(INFO, "idx_touch: initialized empty index file\n");
	}
	if (book_index->used != db_size) { //< out of sync
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
	Book b;

	db_init("books.dat", "books.idx");

	DO(3, next_id())

	UJ delete_test = 10;
	if(rec_delete(delete_test) == NONE)
		T(WARN, "no such record %lu\n", delete_test);
	//db_dump(); idx_dump(0);

	rec_get(&b, 5); //< load from disk
	T(TEST, "\nbefore update: "); rec_print_dbg(&b);
	H pages = 666;
	rec_set(&b, fld_pages, &pages);
	rec_set(&b, fld_title, "WINNIE THE POOH");
	rec_update(&b);
	rec_get(&b, 5); //< reload from disk
	T(TEST, "after update: "); rec_print_dbg(&b); T(TEST, "\n");

	//db_dump(); idx_dump(0);
	DO(3, rec_create(&b);)
	rec_delete(17);
	db_dump(); idx_dump(0);

	//rec_delete(20); //< delete last
	//db_dump(); idx_dump(0);

	idx_close();

	R 0;
}

I main() { R test(); }

//:~