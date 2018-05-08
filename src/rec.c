#include <stdlib.h>
#include <string.h>
#include "books.h"
#include "trc.h"
#include "bin.h"
#include "arr.h"
#include "idx.h"
#include "fio.h"

//! debug print
V rec_print_dbg(Book *b) {
	T(TEST, "rec:\tid=(%lu)\tpages=(%d)\ttitle=(%s)\n", b->book_id, b->pages, b->title);
}

//! find index position by book_id
UJ rec_get_idx_pos(Arr* idx, UJ book_id) {
	R binfn(idx->data, &book_id, Idx, idx->used, (BIN_CMP_FN)&cmp_binsearch);
}

//! substring search in given field
C rec_search_txt_field(V *rec, I fld, S needle) {
	S haystack = (S)rec+rec_field_offsets[fld];
	R !!strcasestr(haystack, needle);
}

//! find database position by book_id
UJ rec_get_db_pos(UJ book_id) {
	UJ idx_pos = rec_get_idx_pos(book_index, book_id);
	if(idx_pos==NONE)R NONE; //< no such book
	Idx *e = arr_at(book_index, idx_pos, Idx);
	T(TRACE, "rec_get_db_pos: { book_id=%lu, idx_pos=%lu, db_pos=%lu }\n", e->book_id, idx_pos, e->pos);
	R e->pos;
}

//! load record by book_id
UJ rec_get(Book *dest, UJ book_id) {
	UJ db_pos = rec_get_db_pos(book_id);
	T(TRACE, "rec_get: { book_id=%lu, db_pos=%lu }\n", book_id, db_pos);
	if(db_pos==NONE)R NONE; //< no such book
	FILE *db = fopen(db_file, "r");
	zseek(db, (db_pos)*SZ(Book), SEEK_SET);
	fread(dest, SZ(Book), 1, db);
	fclose(db);
	R db_pos;
}

//! delete record from db and index
UJ rec_delete(UJ book_id) {
	UJ db_pos = rec_get_db_pos(book_id);
	if(db_pos==NONE)R NONE; //< no such book
	T(DEBUG, "rec_delete: { book_id=%lu, db_pos=%lu }\n", book_id, db_pos);	
	
	FILE *db = fopen(db_file, "r+");

	if (book_index->used > 1){
		Book b;
		UJ last_pos = book_index->used-1;
		J offset = SZ(Book)*last_pos;
		zseek(db, offset, SEEK_SET);
		fread(&b, SZ(Book), 1, db);	//< read last record
		T(TRACE, "rec_delete: loaded tail record { book_id=%lu, pos=%lu, offset=%ld }\n", b.book_id, last_pos, offset);
		zseek(db, db_pos*SZ(Book), SEEK_SET);
		fwrite(&b, SZ(Book), 1, db); //< overwrite deleted record
		idx_update_pos(b.book_id, db_pos);
		idx_update_pos(book_id, NONE);
		T(TRACE, "rec_delete: overwritten record { book_id=%lu, db_pos=%lu }\n", book_id, db_pos);
	}

	UJ new_size = idx_shift(db_pos);
	ftrunc(db, SZ(Book)*new_size);
	T(TRACE, "rec_delete: db file truncated\n");
	fclose(db);
	R db_pos;
}

//! create record
UJ rec_create(Book *b) {
	b->book_id = next_id();
	FILE *db = fopen(db_file, "a");
	UJ db_pos = fsize(db)/SZ(Book);
	fwrite(b, SZ(Book), 1, db);
	fclose(db);
	T(DEBUG, "rec_create: { book_id=%lu, pos=%lu }\n", b->book_id, db_pos);
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
	T(DEBUG, "rec_update: book_id=%lu updated\n", b->book_id);
	R db_pos;
}

//! update field
V rec_set(V*b, I fld, V* val) {
	I offset = rec_field_offsets[fld];
	I len = fld<3?SZ(H)-1:MIN(csv_max_field_widths[fld],strlen(val));
	memcpy(b+offset, val, len+1);
	T(TRACE, "rec_set: fld=%d, len=%d\n", fld, len);
}

//:~