//!\file rec.c \brief database record operations

#include <stdlib.h>
#include <string.h>
#include "___.h"
#include "idx.h"
#include "rec.h"

//! network client only uses rec_set()
#ifndef NET_CLIENT

//! debug print
V rec_print_dbg(Rec r) {
	LOG("rec_print_dbg");
	T(TEST, "id=(%lu) year=(%d) pages=(%d) title=(%s) publisher=(%s)",
		r->rec_id, r->year, r->pages, r->title, r->publisher);
}

//! binary search for index position of rec_id
//! \return NIL if nothing found
UJ rec_get_idx_pos(ID rec_id) {
	R binx(idx_data(), &rec_id, Pair, idx_size(), &cmp_binsearch);
}

//! substring search in a given field \return 1 if match found
C rec_search_txt_field(V*r, I fld, S needle) {
	S haystack = (S)r + rec_field_offsets[fld+1];
	R !!sstr(haystack, needle);
}

//! find database position by rec_id
UJ rec_get_db_pos(ID rec_id) {
	LOG("rec_get_db_pos");

	UJ idx_pos = rec_get_idx_pos(rec_id);
	X(idx_pos==NIL, T(DEBUG, "rec_get_idx_pos returned nil"), NIL);

	Pair *e = idx_get_entry(idx_pos);
	T(TRACE, "{ rec_id=%lu, idx_pos=%lu, db_pos=%lu }", e->rec_id, idx_pos, e->pos);
	R e->pos;
}

//! load record into dest by rec_id
UJ rec_get(Rec dest, ID rec_id) {
	LOG("rec_get");
	UJ db_pos = rec_get_db_pos(rec_id);
	X(db_pos==NIL, T(TEST, "rec_get_db_pos returned nil"), NIL);
	T(TRACE, "{ rec_id=%lu, db_pos=%lu }", rec_id, db_pos);

	FILE* db;
	xfopen(db, db_file, "r", NIL);
	zseek(db, db_pos * SZ_REC, SEEK_SET);
	fread(dest, SZ_REC, 1, db);
	fclose(db);
	R db_pos;
}

//! delete record from db and index
UJ rec_delete(ID rec_id) {
	LOG("rec_delete");

	UJ db_pos = rec_get_db_pos(rec_id);
	X(db_pos==NIL, T(DEBUG,"rec_get_db_pos returned nil"), NIL);

	T(DEBUG, "deleting { rec_id=%lu, db_pos=%lu }", rec_id, db_pos);	
	
	FILE* db;
	xfopen(db, db_file, "r+", NIL);
	UJ last_pos = idx_size()-1;

	if(db_pos==last_pos) { // deleting last record in the file
		T(DEBUG, "deleting tail record in db file");
	} else {
		//! load tail record, paste it in place of the deleted and update its index entry
		Rec b = malloc(SZ_REC); chk(b, NIL);
		J offset = SZ_REC * last_pos;
		zseek(db, offset, SEEK_SET);
		fread(b, SZ_REC, 1, db);	//< read last record
		T(DEBUG, "loaded tail record { rec_id=%lu, pos=%lu, offset=%ld }", b->rec_id, last_pos, offset);
		zseek(db, db_pos * SZ_REC, SEEK_SET);
		fwrite(b, SZ_REC, 1, db); //< overwrite deleted record
		T(DEBUG, "overwritten record { rec_id=%lu, db_pos=%lu }", rec_id, db_pos);
		idx_update_pos(b->rec_id, db_pos);
		free(b);
	}
	//! finally, truncate both index and db file by one pair and one record, respectively
	UJ new_size = idx_shift(rec_get_idx_pos(rec_id));
	ftrunc(db, SZ_REC * new_size);
	T(DEBUG, "db file truncated");
	fclose(db);
	idx_reset_sort_vectors();
	R db_pos;}

//! create a new record and add it to index
UJ rec_create(Rec r) {
	LOG("rec_create");
	r->rec_id = next_id();

	FILE* db;
	xfopen(db, db_file, "a", NIL);
	UJ db_pos = fsize(db)/SZ_REC;
	fwrite(r, SZ_REC, 1, db);
	fclose(db);

	idx_add(r->rec_id, db_pos);	
	T(DEBUG, "created rec_id=%lu, pos=%lu", r->rec_id, db_pos);
	idx_reset_sort_vectors();
	R db_pos;
}

//! update record on disk
UJ rec_update(Rec r) {
	LOG("rec_update");
	UJ db_pos = rec_get_db_pos(r->rec_id);

	X(db_pos==NIL, T(WARN, "rec_get_db_pos reports error"), NIL)

	FILE*db;
	xfopen(db, db_file, "r+", NIL);
	UJ offset = SZ_REC * db_pos;
	zseek(db, offset, SEEK_SET);
	fwrite(r, SZ_REC, 1, db); //< overwrite old version
	fclose(db);

	T(DEBUG, "updated rec_id=%lu", r->rec_id);
	idx_reset_sort_vectors();
	R db_pos;}

#endif

//! update field value
V rec_set(Rec r, I fld, V* val) {
	LOG("rec_set");
	I offset = rec_field_offsets[fld+1]; //< 0 is reserved for rec_id;
	I len = fld<2?SZ(H)-1:MIN(csv_max_field_widths[fld],scnt(val));
	if(fld>=2)r->lengths[fld-2]=len;
	mcpy(((V*)r)+offset, val, len+1);
	T(DEBUG, "rec_id=%lu, fld=%d, len=%d", r->rec_id, fld, len);
}


//:~

