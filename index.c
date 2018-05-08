#include <stdlib.h>
#include <string.h>
#include "books.h"
#include "dynarray.h"
#include "binsearch.h"
#include "index.h"

Z Arr *book_index = 0;
Z C db_file[MAX_FNAME_LEN+1];
Z C idx_file[MAX_FNAME_LEN+1];

#if WIN32||_WIN64
Z J zseek(I d,J j,I f){UI h=(UI)(j>>32),l=SetFilePointer((HANDLE)d,(UI)j,&h,f);R((J)h<<32)+l;}
I ftrunc(FILE*d,UJ n){R zseek((I)d,n,0),SetEndOfFile((HANDLE)d)-1;}
#else
#include <sys/types.h>
#include <unistd.h>
Z J zseek(FILE*d,J j,I f){R fseek(d,j,f);}
I ftrunc(FILE*d,UJ n){R zseek(d,n,0),ftruncate(fileno(d),n);}
#endif

//! filesize utility
Z UJ fsize(FILE *fp) {
	UJ prev = ftell(fp);
	zseek(fp, 0L, SEEK_END);
	UJ sz = ftell(fp);
	zseek(fp, prev, SEEK_SET);
	R sz;
}

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
Z C cmp_binsearch(V*a, V*b, size_t t) {
	J r = _c(a,b);
	//O("index_cmp %lu %lu r=%ld\n", x, y, r);
	R !r?r:r<0?-1:1;
}

//! comparator for qsort()
Z I cmp_qsort(const V*a, const V*b) {
	J r = _c(a,b);
	//O("cmp %lu %lu", x, y);
	R r;
}

//! find index position by book_id
Z UJ rec_get_idx_pos(Arr* idx, UJ book_id) {
	R binfn(idx->data, &book_id, Idx, idx->used, (BIN_CMP_FN)&cmp_binsearch);
}

//! find database position by book_id
Z UJ rec_get_db_pos(UJ book_id) {
	UJ idx_pos = rec_get_idx_pos(book_index, book_id);
	if(idx_pos==NONE)R NONE; //< no such book
	Idx *e = arr_at(book_index, idx_pos, Idx);
	//O("rec_get_db_pos: { book_id=%lu, idx_pos=%lu, db_pos=%lu }\n", e->book_id, idx_pos, e->pos);
	R e->pos;
}

//! debug print
Z V rec_print(Book *b) {
	O("rec:\tid=(%lu)\tpages=(%d)\ttitle=(%s)\n", b->book_id, b->pages, b->title);
}

//! load record by book_id
UJ rec_get(Book *dest, UJ book_id) {
	UJ db_pos = rec_get_db_pos(book_id);
	//O("rec_get: { book_id=%lu, db_pos=%lu }\n", book_id, db_pos);
	if(db_pos==NONE)R NONE; //< no such book
	FILE *db = fopen("books.dat", "r");
	zseek(db, (db_pos)*SZ(Book), SEEK_SET);
	fread(dest, SZ(Book), 1, db);
	fclose(db);
	R db_pos;
}

//! sort index by book_id
Z V idx_sort() {
	qsort(book_index->data, book_index->used, SZ(Idx), cmp_qsort);
	//O("idx_sort: sorted, last_id=%ld\n", last->book_id);
}

//! rebuild index from scratch
V idx_rebuild() {
	FILE *in = fopen(db_file, "r");

	UJ rcnt, pos;
	Book buf[BUFSIZE];

	while((rcnt = fread(buf, SZ(Book), BUFSIZE, in))) {
		//O("idx_rebuild: read %lu records\n", rcnt);
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

	//O("idx_rebuild: rebuilt, entries=%lu, last_id=%lu\n", book_index->used, book_index->hdr);
}

//! free memory
V idx_close() {
	arr_free(book_index);
}

//! zap index entry at pos
Z UJ idx_shift(UJ pos) {
	Idx*s = arr_at(book_index, pos, Idx);
	UJ to_move = book_index->used-pos-1;
	memcpy(s, s+1, SZ(Idx)*to_move);
	//O("idx_shift: shifted %lu entries\n", to_move);
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
	//O("idx_save: %lu bytes\n", size);
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
	//O("idx_load: %lu bytes, %lu entries, capacity=%lu, last_id=%lu\n", size, book_index->used, book_index->size, book_index->hdr);
}

//! update index header on disk
V idx_update_hdr() {
	FILE *out = fopen(idx_file, "r+");
	zseek(out, 0L, SEEK_SET);
	fwrite(book_index, SZ(Arr), 1, out);
	fclose(out);
	//O("idx_update_hdr: idx header updated\n");
}

//! get next available id and store it on disk
UJ next_id() {
	UJ id = ++book_index->hdr;
	idx_update_hdr();
	return id;
}

//! patch record's pos pointer and store it on disk
Z UJ idx_update_pos(UJ book_id, UJ new_pos) {
	UJ idx_pos = rec_get_idx_pos(book_index, book_id);
	if(idx_pos==NONE)R NONE; //< no such book
	Idx *i = arr_at(book_index, idx_pos, Idx);
	i->pos = new_pos;
	FILE *out = fopen(idx_file, "r+");
	zseek(out, SZ(Arr)+SZ(Idx)*(idx_pos-1), SEEK_SET);
	fwrite(i, SZ(Idx), 1, out);
	fclose(out);
	//O("idx_update_pos: { book_id=%lu, new_pos=%lu }\n", book_id, new_pos);
	R new_pos;
}

//! delete record from db and index
UJ rec_delete(UJ book_id) {
	UJ db_pos = rec_get_db_pos(book_id);
	if(db_pos==NONE)R NONE; //< no such book
	//O("rec_delete: { book_id=%lu, db_pos=%lu }\n", book_id, db_pos);	
	
	FILE *db = fopen(db_file, "r+");

	if (book_index->used > 1){
		Book b;
		UJ last_pos = book_index->used-1;
		J offset = SZ(Book)*last_pos;
		zseek(db, offset, SEEK_SET);
		J pos = ftell(db);
		fread(&b, SZ(Book), 1, db);	//< read last record
		//O("rec_delete: loaded tail record { book_id=%lu, pos=%lu, offset=%ld ftell=%ld }\n", b.book_id, last_pos, offset, pos);
		//rec_print(&b);
		zseek(db, db_pos*SZ(Book), SEEK_SET);
		fwrite(&b, SZ(Book), 1, db); //< overwrite deleted record
		//O("rec_delete: overwritten record { book_id=%lu, db_pos=%lu }\n", book_id, db_pos);
		idx_update_pos(b.book_id, db_pos);
		idx_update_pos(book_id, NONE);
	}

	UJ new_size = idx_shift(db_pos);
	ftrunc(db, SZ(Book)*new_size);
	//O("rec_delete: db file truncated\n");
	fclose(db);

	R db_pos;
}

Z V idx_add(UJ book_id, UJ pos) {
	Idx e;
	e.book_id = book_id;
	e.pos = pos;
	arr_add(book_index, e);
	//O("idx_add: { book_id=%lu, pos=%lu }\n", book_id, pos);
	idx_save();
}

UJ rec_create(Book *b) {
	b->book_id = next_id();
	FILE *db = fopen(db_file, "a");
	UJ db_pos = fsize(db)/SZ(Book);
	fwrite(b, SZ(Book), 1, db);
	fclose(db);
	//O("rec_create: { book_id=%lu, pos=%lu }\n", b->book_id, db_pos);
	idx_add(b->book_id, db_pos);
	R db_pos;
}

//! update record
UJ rec_update(Book *b) {
	UJ db_pos = rec_get_db_pos(b->book_id);
	if(db_pos==NONE)R NONE; //< no such book
	FILE *db = fopen(db_file, "r+");
	J offset = SZ(Book)*db_pos;
	zseek(db, offset, SEEK_SET);
	fwrite(b, SZ(Book), 1, db); //< overwrite old data
	fclose(db);
	//O("rec_update: book_id=%lu updated\n", b->book_id);
	R db_pos;
}

//! perform sample index lookup
Z UJ idx_peek(UJ book_id){
	Book b;
	UJ pos = rec_get(&b, book_id);
	if (pos==NONE)R NONE;
	//O("idx_peek book_id=%lu pos=%ld\n", book_id, pos);
	rec_print(&b);
	R pos;
}

//! dump index to stdout
Z V idx_dump(UJ head) {
	Idx *e;
	O("\nidx_dump: { last_id=%lu, used=%lu } =>\n\t", book_index->hdr, book_index->used);
	DO(head?head:book_index->used,
		e = arr_at(book_index, i, Idx);
		O(" (%lu->%lu)", e->book_id, e->pos);
	)
	O("\n\n");
}

//! dump db to stdout
Z V db_dump() {
	FILE *in = fopen(db_file, "r");
	Book buf[BUFSIZE];
	UJ rcnt;
	while((rcnt = fread(buf, SZ(Book), BUFSIZE, in)))
		DO(rcnt, rec_print(&buf[i]);)
	fclose(in);
}

//! update field
V rec_set(V*b, I fld, V* val) {
	I offset = rec_field_offsets[fld];
	I len = fld<3?SZ(H)-1:MIN(csv_max_field_widths[fld],strlen(val));
	memcpy(b+offset, val, len+1);
	//O("rec_set: fld=%d, len=%d\n", fld, len);
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
		//O("idx_touch: initialized empty index file\n");
	}
	if (book_index->used != db_size) { //< out of sync
		idx_rebuild();
		idx_save();
		//O("idx_touch: synchronized index file\n");
	}
	fclose(f);
}


V db_init(S d, S i) {
	scpy_s(db_file, d, MAX_FNAME_LEN);
	scpy_s(idx_file, i, MAX_FNAME_LEN);
	idx_touch();
	O("db_init: database initialized\n");
}

I test() {
	Book b;

	db_init("books.dat", "books.idx");

	DO(3, next_id())

	UJ delete_test = 10;
	if(rec_delete(delete_test) == NONE)
		O("no such record %lu\n", delete_test);
	//db_dump(); idx_dump(0);

	rec_get(&b, 5); //< load from disk
	O("\nbefore update: "); rec_print(&b);
	H pages = 666;
	rec_set(&b, fld_pages, &pages);
	rec_set(&b, fld_title, "WINNIE THE POOH");
	rec_update(&b);
	rec_get(&b, 5); //< reload from disk
	O("after update: "); rec_print(&b); O("\n");

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