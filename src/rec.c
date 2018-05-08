#include <stdlib.h>
#include <string.h>
#include "___.h"
#include "cfg.h"
#include "trc.h"
#include "bin.h"
#include "arr.h"
#include "idx.h"
#include "fio.h"

//! debug print
V rec_print_dbg(Rec r) {
	LOG("rec_print_dbg");
	T(TEST, "id=(%lu) pages=(%d) title=(%s)", r->rec_id, r->pages, r->title);
}

//! binary search for index position of rec_id
UJ rec_get_idx_pos(ID rec_id) {
	R binfn(idx_data(), &rec_id, Pair, idx_size(), &cmp_binsearch);
}

//! substring search in given field
C rec_search_txt_field(V*rec, I fld, S needle) {
	S haystack = (S)rec+rec_field_offsets[fld];
	R !!strcasestr(haystack, needle);
}

//! find database position by rec_id
UJ rec_get_db_pos(ID rec_id) {
	LOG("rec_get_db_pos");
	UJ idx_pos = rec_get_idx_pos(rec_id);
	BAIL_IF(idx_pos, NONE);
	Pair *e = idx_get_entry(idx_pos);
	T(TRACE, "rec_get_db_pos: { rec_id=%lu, idx_pos=%lu, db_pos=%lu }\n", e->rec_id, idx_pos, e->pos);
	R e->pos;
}

//! load record into dest by rec_id
UJ rec_get(Rec dest, UJ rec_id) {
	LOG("rec_get");
	UJ db_pos = rec_get_db_pos(rec_id);
	T(TRACE, "rec_get: { rec_id=%lu, db_pos=%lu }\n", rec_id, db_pos);
	BAIL_IF(db_pos, NONE);
	FILE *db = fopen(db_file, "r");
	zseek(db, db_pos*SZ_REC, SEEK_SET);
	fread(dest, SZ_REC, 1, db);
	fclose(db);
	R db_pos;
}

//! delete record from db and index
UJ rec_delete(UJ rec_id) {
	LOG("rec_delete");
	UJ db_pos = rec_get_db_pos(rec_id);
	BAIL_IF(db_pos, NONE);
	T(DEBUG, "rec_delete: { rec_id=%lu, db_pos=%lu }\n", rec_id, db_pos);	
	
	FILE *db = fopen(db_file, "r+");

	Rec b = malloc(SZ_REC);
	UJ last_pos = idx_size()-1;
	J offset = SZ_REC*last_pos;
	zseek(db, offset, SEEK_SET);
	fread(b, SZ_REC, 1, db);	//< read last record
	T(TRACE, "rec_delete: loaded tail record { rec_id=%lu, pos=%lu, offset=%ld }\n", b->rec_id, last_pos, offset);
	zseek(db, db_pos*SZ_REC, SEEK_SET);
	fwrite(b, SZ_REC, 1, db); //< overwrite deleted record
	idx_update_pos(b->rec_id, db_pos);
	idx_update_pos(rec_id, NONE);
	T(TRACE, "rec_delete: overwritten record { rec_id=%lu, db_pos=%lu }\n", rec_id, db_pos);
	free(b);

	UJ new_size = idx_shift(db_pos);
	ftrunc(db, SZ_REC*new_size);
	T(TRACE, "rec_delete: db file truncated\n");
	fclose(db);
	R db_pos;
}

//! create record
UJ rec_create(Rec r) {
	LOG("rec_create");
	r->rec_id = next_id();
	FILE *db = fopen(db_file, "a");
	UJ db_pos = fsize(db)/SZ_REC;
	fwrite(r, SZ_REC, 1, db);
	fclose(db);
	T(DEBUG, "rec_create: { rec_id=%lu, pos=%lu }\n", r->rec_id, db_pos);
	idx_add(r->rec_id, db_pos);
	R db_pos;
}

//! update record
UJ rec_update(Rec r) {
	LOG("rec_update");
	UJ db_pos = rec_get_db_pos(r->rec_id);
	BAIL_IF(db_pos, NONE);
	FILE *db = fopen(db_file, "r+");
	UJ offset = SZ_REC*db_pos;
	zseek(db, offset, SEEK_SET);
	fwrite(r, SZ_REC, 1, db); //< overwrite old data
	fclose(db);
	T(DEBUG, "rec_update: { rec_id=%lu }\n", r->rec_id);
	R db_pos;
}

//! update field
V rec_set(Rec r, I fld, V*val) {
	LOG("rec_set");
	I offset = rec_field_offsets[fld];
	I len = fld<3?SZ(H)-1:MIN(csv_max_field_widths[fld],strlen(val));
	memcpy(((V*)r)+offset, val, len+1);
	T(TRACE, "rec_set: { rec_id=%lu, fld=%d, len=%d }\n", r->rec_id, fld, len);
}


//:~