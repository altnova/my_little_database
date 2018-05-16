//!\file idx.c \brief index file operations

#include <stdlib.h>
#include <string.h>
#include "___.h"
#include "cfg.h"
#include "trc.h"
#include "arr.h"
#include "bin.h"
#include "idx.h"
#include "rec.h"
#include "fio.h"
#include "clk.h"

Z Arr idx;		//< in-memory instance of the index
Z bufRec buf;	//< readbuffer RECBUFLEN records

C db_file[MAX_FNAME_LEN+1];
C idx_file[MAX_FNAME_LEN+1];

//! current idx size
UJ idx_size() {
	R arr_size(idx);
}

//! returns specific index entry
Pair* idx_get_entry(UJ idx_pos) {
	R arr_at(idx, idx_pos, Pair);
}

//! persist index in a file
//! \return NIL on error, index bytesize on success
Z UJ idx_save() {
	LOG("idx_save");
	FILE*out;
	xfopen(out, idx_file, "w+", NIL);
	fwrite(idx, SZ(Arr)+idx->size*SZ(Pair), 1, out); //< TODO check error
	UJ idx_fsize = fsize(out);
	fclose(out);
	T(TRACE, "saved %lu bytes", idx_fsize);
	R idx_fsize;
}

//! squash index entry at given pos
//! \return NIL on error, new index size on success
UJ idx_shift(UJ pos) {
	LOG("idx_shift");
	Pair* s = arr_at(idx, pos, Pair);
	UJ to_move = idx->used-pos-1;
	memcpy(s, s+1, SZ(Pair) * to_move);
	T(DEBUG, "shifted %lu entries, squashed db_pos=%lu", to_move, pos);

	idx->used--;
	UJ b_written = idx_save();
	X(b_written==NIL, T(WARN, "idx_save failed"), NIL);

	R idx_size();
}

//! returns pointer to indexes' data section
Pair* idx_data() {
	R(Pair*)idx->data;
}

//! create db file if not exists \returns number of records
Z UJ db_touch(S fname) {
	LOG("db_touch");
	FILE* f;
	xfopen(f, fname, "a", NIL);
	UJ size = fsize(f)/SZ_REC;
	fclose(f);
	R size;
}

//! comparator kernel
ZJ _c(const V*a, const V*b) {
	ID x = ((Pair*)a)->rec_id;
	ID y = ((Pair*)b)->rec_id;
	R x-y;
}

//! comparator for binfn()
C cmp_binsearch(V* a, V* b, sz t) {
	LOG("cmp_binsearch");
	J r = _c(a,b);
	T(TRACE, "r=%ld\n", r);
	R !r?r:r<0?-1:1;
}

//! comparator for qsort()
ZI cmp_qsort(const V* a, const V* b) {
	LOG("cmp_qsort");
	J r = _c(a,b);
	T(TRACE, "%ld\n", r);
	R r;
}

//! sort index by rec_id
ZV idx_sort() {
	LOG("idx_sort");
	qsort(idx->data, idx->used, SZ(Pair), cmp_qsort);
	T(DEBUG, "index sorted");
}

//! dump index to stdout
ZV idx_dump(UJ head) {
	LOG("idx_dump");
	Pair* e;
	TSTART();
	T(TEST, "{ last_id=%lu, used=%lu } =>", idx->hdr, idx_size());
	DO(head?head:idx_size(),
		e = arr_at(idx, i, Pair);
		T(TEST, " (%lu -> %lu)", e->rec_id, e->pos);
	)
	if(head&&head<idx_size())T(TEST,"...");
	TEND();
}

UJ idx_each(IDX_EACH fn, V*arg, C halt_on_err) {
	LOG("idx_each")
	FILE* in;
	xfopen(in, db_file, "r", NIL);
	UJ rcnt, pos = 0;
	W((rcnt = fread(buf, SZ_REC, RECBUFLEN, in))) {
		T(DEBUG, "read %lu records", rcnt);
		DO(rcnt,
			Rec b = &buf[i];
			UJ res = fn(b, arg, pos++);
			P(halt_on_err&&res==NIL, NIL)
		)
	}
	R pos;
}


//! rebuild index from scratch
//! \return # recs loaded, NIL on error
Z UJ idx_rebuild() {
	LOG("idx_rebuild");
	FILE* in;
	xfopen(in, db_file, "r", NIL);

	UJ rcnt, pos=0;
	Pair e;

	W((rcnt = fread(buf, SZ_REC, RECBUFLEN, in))) {
		T(DEBUG, "read %lu records", rcnt);
		DO(rcnt,
			Rec b = &buf[i];
			e.rec_id = b->rec_id;
			e.pos = pos++;
			arr_add(idx, e);
		)
	}

	fclose(in);
	idx_sort();

	Pair last = e;
	idx->hdr = last.rec_id; 	//< use Arr header field to store last_id

	T(INFO, "rebuilt, entries=%lu, last_id=%lu", idx->used, idx->hdr);
	R idx_size();
}

//! free memory
ZV idx_close() {
	arr_destroy(idx);
}

//! load index from a file
//! \return number of entries, NIL on error
Z UJ idx_load() {
	LOG("idx_load");
	FILE* in;
	xfopen(in, idx_file, "r", NIL);
	idx_close();
	J size = fsize(in);
	Arr* tmp = &idx; //< replace pointer
	*tmp = arr_init((size-SZ_HDR)/SZ(Pair), Pair); //< reinit idx
	fread(idx, size, 1, in); //< TODO check error
	fclose(in);
	T(INFO, "%lu bytes, %lu entries, capacity=%lu, last_id=%lu", \
		size, idx->used, idx->size, idx->hdr);
	R idx_size();
}

//! update index header on disk
UJ idx_update_hdr() {
	LOG("idx_update_hdr");
	FILE*out;
	xfopen(out, idx_file, "r+", NIL);
	zseek(out, 0L, SEEK_SET);
	fwrite(idx, SZ(Arr), 1, out);
	fclose(out);
	T(DEBUG, "idx header updated");
	R 0;
}

//! get next available id and store it on disk
UJ next_id() {
	ID id = ++idx->hdr;
	idx_update_hdr();
	R id;
}

//! patch record's pos pointer and store it on disk
UJ idx_update_pos(UJ rec_id, UJ new_pos) {
	LOG("idx_update_pos");
	UJ idx_pos = rec_get_idx_pos(rec_id);
	X(idx_pos==NIL, T(WARN, "rec_get_idx_pos failed"), NIL); //< no such record
	Pair *i = arr_at(idx, idx_pos, Pair);
	i->pos = new_pos;

	FILE*out;
	xfopen(out, idx_file, "r+", NIL);
	zseek(out, SZ(Arr)+SZ(Pair)*(idx_pos-1), SEEK_SET);
	fwrite(i, SZ(Pair), 1, out);
	fclose(out);
	T(DEBUG, "rec_id=%lu, new_pos=%lu", rec_id, new_pos);
	R new_pos;
}

//! add new index element and save
UJ idx_add(ID rec_id, UJ db_pos) {
	LOG("idx_add");
	Pair e = (Pair){0,0};
	e.rec_id = rec_id;
	e.pos = db_pos;
	arr_add(idx, e);
	T(DEBUG, "rec_id=%lu, pos=%lu", rec_id, db_pos);
	R idx_save(); //< TODO append single item instead of full rewrite
}

//! perform sample index lookup
Z UJ idx_peek(ID rec_id){
	LOG("idx_peek");
	Rec b;
	UJ pos = rec_get(b, rec_id);
	X(pos==NIL, T(WARN,"rec_get failed"), NIL); //< no such record
	T(TRACE, "rec_id=%lu pos=%ld", rec_id, pos);
	rec_print_dbg(b);
	R pos;
}

//! dump db to stdout
Z UJ db_dump() {
	LOG("db_dump");
	FILE*in;
	xfopen(in, db_file, "r", NIL);
	UJ rcnt;
	while((rcnt = fread(buf, SZ_REC, RECBUFLEN, in))) {
		T(DEBUG, "read %lu records", rcnt);
		DO(rcnt, rec_print_dbg(&buf[i]);)
	}
	fclose(in);
	R 0;
}

//! check the environment and create/rebuild index if necessary
//! \return NIL if error, record count on success
Z UJ idx_open() {
	LOG("idx_open");
	UJ db_size = db_touch(db_file);
	X(db_size==NIL, T(WARN, "db_touch(%s) reports error", db_file), NIL);

	FILE*in;
	xfopen(in, idx_file, "a+", NIL); //< open or create

	UJ idx_fsize = fsize(in);
	X(idx_fsize==NIL, T(WARN, "fsize(%d) reports error", idx_file), NIL);
	fclose(in);

	idx = arr_init(RECBUFLEN, Pair);
	X(idx==NULL, T(WARN, "cannot initialize memory for index"), NIL);

	if (!idx_fsize) { //! empty index
		UJ b_written = idx_save();
		X(b_written==NIL, T(WARN, "idx_save reports error"), NIL);
		T(WARN, "initialized empty index file");
	}

	UJ idx_count = idx_load();
	X(idx_count==NIL, T(WARN, "idx_load reports error"), NIL);

	if (idx_count != db_size) { //< index out of sync
		T(WARN,"index file out of sync idx_size=%lu, db_size=%lu", idx_count, db_size);
		idx_count = idx_rebuild();
		X(idx_count==NIL, T(WARN, "idx_rebuild reports error"), NIL);
		UJ b_written = idx_save();
		X(b_written==NIL, T(WARN,"idx_save reports error"), NIL);
		T(WARN, "synchronized index file");
	} else
		T(WARN, "loaded existing index, idx_size=%lu", idx_count);

	R idx_size();
}

UJ db_init(S d, S i) {
	LOG("db_init");
	scpy(db_file, d, MAX_FNAME_LEN);
	scpy(idx_file, i, MAX_FNAME_LEN);
	UJ idx_size = idx_open();
	T(INFO, "database initialized");
	R idx_size;
}

V db_close() {
	idx_close();
}

#ifdef RUN_TESTS_IDX
ZI idx_test() {
	LOG("idx_test");
	Rec b=malloc(SZ_REC);chk(b,1);

	db_init(DAT_FILE, IDX_FILE);

	DO(3, next_id())

	ID delete_test = 10;
	if(rec_delete(delete_test) == NIL)
		T(WARN, "no such record=%lu", delete_test);

	rec_get(b, 5); //< load from disk
	T(TEST, "before update: "); rec_print_dbg(b);
	H pages = 666;
	rec_set(b, fld_pages, &pages);
	rec_set(b, fld_title, "WINNIE THE POOH");
	rec_update(b);
	rec_get(b, 5); //< reload from disk
	T(TEST, "after update: "); rec_print_dbg(b);

	//db_dump(); idx_dump(0);
	DO(3, rec_create(b);)
	rec_delete(17);
	//db_dump();
	idx_dump(10);

	//rec_delete(20); //< delete last
	//db_dump(); idx_dump(0);

	db_close();
	free(b);
	R0;
}

I main() { R idx_test(); }
#endif

//:~