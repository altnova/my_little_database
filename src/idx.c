//!\file idx.c \brief index file operations

#include <stdlib.h>
#include <string.h>
#include "___.h"
#include "idx.h"

Z IDX idx;		//< in-memory instance of the index
Z bufRec buf;	//< readbuffer RECBUFLEN records

C db_file[MAX_FNAME_LEN+1];
C idx_file[MAX_FNAME_LEN+1];

//! current idx size
UJ idx_size() {
	R vec_size(idx->pairs);
}

//! returns specific index entry
Pair* idx_get_entry(UJ idx_pos) {
	R vec_at(idx->pairs, idx_pos, Pair);
}

//! persist index in a file
//! \return NIL on error, index bytesize on success
Z UJ idx_save() {
	LOG("idx_save");
	FILE*out;
	xfopen(out, idx_file, "w+", NIL);
	vec_compact(&idx->pairs);
	fwrite(idx, SZ_IDX, 1, out);
	fwrite(idx->pairs, vec_mem(idx->pairs), 1, out);
	UJ idx_fsize = fsize(out);
	fclose(out);
	T(DEBUG, "saved %lu+%lu = %lu bytes", SZ_IDX, vec_mem(idx->pairs), idx_fsize);
	R idx_fsize;
}

//! squash index entry at given pos
//! \return NIL on error, new index size on success
UJ idx_shift(UJ pos) {
	LOG("idx_shift");
	vec_del_at(idx->pairs, pos, 1);
	T(DEBUG, "squashed idx_pos=%lu", pos);
	UJ b_written = idx_save();
	X(b_written==NIL, T(WARN, "idx_save failed"), NIL);
	R idx_size();
}

//! returns pointer to indexes' data section
Pair* idx_data() {
	R(Pair*)idx->pairs->data;
}

//! create db file if not exists \returns number of records
Z UJ db_touch(S fname) {
	LOG("db_touch");
	FILE*f;
	xfopen(f, fname, "a", NIL);
	UJ size = fsize(f)/SZ_REC;
	fclose(f);
	R size;
}

//! comparator kernel
ZJ _c(const V*a, const V*b) {
	ID x = ((Pair*)a)->rec_id;
	ID y = ((Pair*)b)->rec_id;
	P(x==y,0)
	R x>y?1:-1;
}

//! comparator for binfn()
C cmp_binsearch(V* a, V* b, sz t) {
	LOG("cmp_binsearch");
	J r = _c(a,b);
	T(TRACE, "r=%ld\n", r);
	R !r?r:r<0?-1:1;
}

//! comparator for qsort()
ZI cmp_qsort(const V*a, const V*b) {
	LOG("cmp_qsort");
	J r = _c(a,b);
	T(TRACE, "%ld\n", r);
	R(I)r;
}

//! sort index by rec_id
ZV idx_sort() {
	LOG("idx_sort");
	VEC p = idx->pairs;
	qsort(p->data, p->used, SZ(Pair), cmp_qsort);
	T(DEBUG, "index sorted");
}

//! dump index to stdout
V idx_dump(UJ head) {
	LOG("idx_dump");
	Pair* e;
	TSTART();
	T(TEST, "{ last_id=%lu, used=%lu, size=%lu } =>", idx->last_id, idx_size(), idx->pairs->size);
	DO(head?head:idx_size(),
		e = vec_at(idx->pairs, i, Pair);
		//if (!e->rec_id)
			T(TEST, " %lu:(%lu -> %lu)", i, e->rec_id, e->pos);
	)
	if(head&&head<idx_size())T(TEST,"...");
	TEND();
}

UJ idx_each(IDX_EACH fn, V*arg) {
	LOG("idx_each")
	FILE*in;
	xfopen(in, db_file, "r", NIL);
	UJ rcnt, pos = 0;
	W((rcnt = fread(buf, SZ_REC, RECBUFLEN, in))) {
		T(DEBUG, "read %lu records", rcnt);
		DO(rcnt,
			Rec b = &buf[i];
			UJ res = fn(b, arg, pos++);
		)
	}
	fclose(in);
	R pos;
}

UJ idx_page(IDX_EACH fn, V*arg, I page, I page_sz) {
	LOG("idx_page")
	FILE* in;
	xfopen(in, db_file, "r", NIL);
	UJ fpos = SZ_REC * page * page_sz;
	zseek(in, fpos, SEEK_SET);
	UJ rcnt, pos = 0;
	rcnt = fread(buf, SZ_REC, page_sz, in);
	T(DEBUG, "read page: %lu records", rcnt);
	DO(rcnt,
		Rec b = &buf[i];
		UJ res = fn(b, arg, pos++);
	)
	fclose(in);
	R pos;
}

//! rebuild index from scratch
//! \return # recs loaded, NIL on error
Z UJ idx_rebuild() {
	LOG("idx_rebuild");
	FILE*in;
	xfopen(in, db_file, "r", NIL);

	UJ rcnt, pos=0;
	Pair e;

	W((rcnt = fread(buf, SZ_REC, RECBUFLEN, in))) {
		T(DEBUG, "read %lu records", rcnt);
		DO(rcnt,
			Rec b = &buf[i];
			e.rec_id = b->rec_id;
			e.pos = pos++;
			vec_add(idx->pairs, e);)
	}

	fclose(in);

	idx_sort();
	// after sorting, it is easy to recover last_id
	e = *vec_last(idx->pairs,Pair);
	idx->last_id = e.rec_id;

	T(INFO, "rebuilt, entries=%lu, last_id=%lu", idx_size(), idx->last_id);
	R idx_size();
}

//! free memory
Z sz idx_close() {
	sz mem = vec_destroy(idx->pairs);
	free(idx);
	R SZ_IDX+mem;
}

//! map index into memory from file
//! \return number of entries, NIL on error
Z UJ idx_load() {
	LOG("idx_load");
	FILE*in;
	xfopen(in, idx_file, "r", NIL);
	idx_close();
	J size = fsize(in);
	idx = malloc(SZ_IDX); chk(idx, NIL);
	fread(idx, SZ_IDX, 1, in);
	idx->pairs = malloc(size-SZ_IDX); chk(idx->pairs, NIL);
	fread(idx->pairs, size-SZ_IDX, 1, in);
	fclose(in);
	VEC p = idx->pairs;
	T(INFO, "%lu bytes, %lu entries, capacity=%lu, last_id=%lu", \
		size, p->used, p->size, idx->last_id);

	R idx_size();
}

//! update index header on disk
UJ idx_update_hdr() {
	LOG("idx_update_hdr");
	FILE*out;
	xfopen(out, idx_file, "r+", NIL);
	fwrite(idx, SZ_IDX, 1, out);
	fclose(out);
	T(DEBUG, "idx header updated");
	R0;
}

//! get next available id and store it on disk
UJ next_id() {
	ID id = ++idx->last_id;
	idx_update_hdr();
	R id;
}

Z UJ last_id() {
	R idx->last_id;
}


//! patch record's pos pointer and store it on disk
UJ idx_update_pos(UJ rec_id, UJ new_pos) {
	LOG("idx_update_pos");
	UJ idx_pos = rec_get_idx_pos(rec_id);
	X(idx_pos==NIL, T(WARN, "rec_get_idx_pos failed"), NIL); //< no such record
	Pair *i = vec_at(idx->pairs, idx_pos, Pair);
	i->pos = new_pos;

	FILE*out;
	xfopen(out, idx_file, "r+", NIL);
	zseek(out, SZ_IDX + SZ_VEC + SZ(Pair) * (idx_pos-1), SEEK_SET);
	fwrite(i, SZ(Pair), 1, out);
	fclose(out);
	T(DEBUG, "rec_id=%lu, idx_pos=%lu, new_pos=%ld", rec_id, idx_pos, new_pos==NIL?-1:(J)new_pos);
	R new_pos;
}

//! add new index element and save
UJ idx_add(ID rec_id, UJ db_pos) {
	LOG("idx_add");
	Pair e = (Pair){0,0};
	e.rec_id = rec_id;
	e.pos = db_pos;
	vec_add(idx->pairs, e);
	T(DEBUG, "rec_id=%lu, pos=%lu", rec_id, db_pos);

	P(idx_size()==1, idx_save()) //< special case

	// append to index file
	FILE*out;
	xfopen(out, idx_file, "r+", NIL);
	UJ old_fsize = fsize(out);
	zseek(out, SZ_IDX, SEEK_SET);
	fwrite(idx->pairs, SZ_VEC, 1, out); //< update vec header
	zseek(out, old_fsize, SEEK_SET); //< go to the end
	fwrite(&e, SZ(Pair), 1, out); //< write new pair
	UJ new_fsize = fsize(out);
	fclose(out);
	T(DEBUG, "appended index entry, old fsize=%lu, new fsize=%lu", old_fsize, new_fsize);
	R new_fsize;
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

Z sz idx_dbsize() {
	LOG("idx_dbsize");
	FILE*db;
	xfopen(db,db_file,"r",NIL);
	sz db_sz = fsize(db);
	fclose(db);
	R db_sz;
}

Z sz idx_fsize() {
	LOG("idx_fsize");
	FILE*idxh;
	xfopen(idxh,idx_file,"r",1);
	sz fsz = fsize(idxh);
	fclose(idxh);
	R fsz;
}

//! dump db to stdout
Z UJ db_dump() {
	LOG("db_dump");
	FILE*in;
	xfopen(in, db_file, "r", NIL);
	UJ rcnt;
	W((rcnt = fread(buf, SZ_REC, RECBUFLEN, in))) {
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

	idx = (IDX)malloc(SZ_IDX);chk(idx,NIL);
	idx->last_id = 0;
	idx->pairs = vec_init(RECBUFLEN, Pair);
	X(idx->pairs==NULL, T(WARN, "cannot initialize memory for index"), NIL);

	if (!idx_fsize) { //! empty index
		UJ b_written = idx_save();
		X(b_written==NIL, T(WARN, "idx_save reports error"), NIL);
		T(INFO, "initialized empty index file");
	}

	UJ idx_count = idx_load();
	X(idx_count==NIL, T(WARN, "idx_load reports error"), NIL);

	if (idx_count != db_size) { //< index out of sync
		T(DEBUG,"index file out of sync idx_size=%lu, db_size=%lu", idx_count, db_size);
		idx_count = idx_rebuild();
		X(idx_count==NIL, T(WARN, "idx_rebuild reports error"), NIL);
		UJ b_written = idx_save();
		X(b_written==NIL, T(WARN,"idx_save reports error"), NIL);
		T(WARN, "synchronized index file");
	} else
		T(INFO, "loaded existing index, idx_size=%lu", idx_count);

	R idx_size();
}

UJ db_init(S d, S i) {
	LOG(" db_init");
	scpy(db_file, d, MAX_FNAME_LEN);
	scpy(idx_file, i, MAX_FNAME_LEN);
	UJ idx_size = idx_open();
	T(DEBUG, "database initialized");
	R SZ_IDX + vec_mem(idx->pairs);
}

sz db_close() {
	R idx_close();
}

#ifdef RUN_TESTS_IDX
ZI idx_test() {
	LOG("idx_test");
	Rec r=malloc(SZ_REC);chk(r,1);
	Rec r1=malloc(SZ_REC);chk(r1,1);
	//db_init(DAT_FILE, IDX_FILE);

	S test_dbfile = "dat/test.dat";
	S test_idxfile = "dat/test.idx";

	ASSERT(!fexist(test_dbfile), "test db file shouldn't exist")
	ASSERT(!fexist(test_idxfile), "test idx file shouldn't exist")

	scpy(db_file, test_dbfile, MAX_FNAME_LEN);
	scpy(idx_file, test_idxfile, MAX_FNAME_LEN);

	UJ idx_sz = idx_open();

	ASSERT(fexist(test_dbfile), "test db file should now be created")
	ASSERT(fexist(test_idxfile), "test idx file should now be created")

	ASSERT(idx_dbsize()==0, "empty db file size should be 0")

	I predict = SZ_IDX+SZ_VEC+SZ(Pair)*1;
	ASSERT(idx_fsize()==predict, "empty idx file size should match expected")

	r->year = 2000;
	r->pages = 1;
	scpy(r->author, "author1", 7);
	scpy(r->publisher, "publisher1", 10);
	scpy(r->title, "title1", 6);
	scpy(r->subject, "subject1", 8);
	ASSERT(rec_create(r)==0, "1st record should be at dbpos=0")

	r->year = 2001;
	r->pages = 2;
	scpy(r->author, "author2", 7);
	scpy(r->publisher, "publisher2", 10);
	scpy(r->title, "title2", 6);
	scpy(r->subject, "subject2", 8);
	ASSERT(rec_create(r)==1, "2st record should be at dbpos=1")

	r->year = 2002;
	r->pages = 3;
	scpy(r->author, "author3", 7);
	scpy(r->publisher, "publisher3", 10);
	scpy(r->title, "title3", 6);
	scpy(r->subject, "subject3", 8);
	ASSERT(rec_create(r)==2, "3rd record should be at dbpos=2")

	ASSERT(idx_size()==3, "idx_size() should now be 3")
	ASSERT(last_id()==3, "last_id should now be 3")

	Pair*p;
	I total = 0;
	DO(idx_size(), p = idx_get_entry(i); total += p->rec_id==i+1&&p->pos==i)
	ASSERT(total==3, "index should be consistent at this point");

	rec_delete(1);
	ASSERT(idx_size()==2, "idx_size() should now be 2")

	UJ dbpos = rec_get_db_pos(3);
	ASSERT(dbpos==0, "tail record should have moved to dbpos 0");

	ASSERT(idx_dbsize()==2*SZ_REC, "db should be truncated on deletion (#1)")
	ASSERT(rec_delete(1)==NIL, "deleting non-existent record should return NIL")

	rec_delete(3);
	ASSERT(idx_size()==1, "idx_size() should now be 1")
	ASSERT(idx_dbsize()==SZ_REC, "db should be truncated on deletion (#2)")

	rec_delete(2);
	ASSERT(idx_size()==0, "idx_size() should now be 0")
	ASSERT(idx_dbsize()==0, "db should be empty on deletion of last record")

	rec_create(r);

	predict = SZ_IDX+SZ_VEC+SZ(Pair)*1;
	ASSERT(idx_fsize()==predict, "idx file size should match expected")

	idx_close();
	idx_open();

	ASSERT(last_id()==4, "last_id() should survive index reopen")

	ASSERT(rec_create(r)==1, "1st record should be at dbpos=0")
	ASSERT(rec_create(r)==2, "2nd record should be at dbpos=1")

	//idx_dump(0);

	predict = SZ_IDX+SZ_VEC+SZ(Pair)*3;
	ASSERT(idx_fsize()==predict, "idx file size should match expected")

	ASSERT(rec_get(r1, 5)==1, "rec_get(5) should report dbpos=1")

	ASSERT(!scmp(r1->publisher, r->publisher), "record data should match expected (#1)")
	ASSERT(!scmp(r1->title, r->title), "record data should match expected (#2)")
	ASSERT(!scmp(r1->subject, r->subject), "record data should match expected (#3)")

	UH year=2018, pages=777;
	rec_set(r1, 0, &year);
	rec_set(r1, 1, &pages);
	rec_set(r1, 2, "Hachette");
	rec_set(r1, 3, "WINNIE THE POOH");
	rec_set(r1, 4, "Alan Milne");
	rec_set(r1, 5, "Winnie-the-Pooh, also called Pooh Bear, is a fictional anthropomorphic teddy bear created by English author A. A. Milne.");

	rec_update(r1);
	// wipe record
	free(r1); r1 = (Rec)calloc(SZ_REC,1);chk(r1,1);

	idx_close();

	FILE*idxh;
	xfopen(idxh,test_idxfile,"r+",1);
	ftrunc(idxh,0); // truncate to test idx_rebuld on idx_open()
	fclose(idxh);

	idx_open();

	predict = SZ_IDX+SZ_VEC+SZ(Pair)*3;
	ASSERT(idx_fsize()==predict, "idx file size should match expected after rebuild")
	ASSERT(last_id()==6, "last_id() should match expected after rebuild")

	ASSERT(rec_get(r1, 5)==1, "rec_get should report dbpos=1")

	//rec_print_dbg(r1);

	ASSERT(!scmp(r1->author, "Alan Milne"), "record data should match expected (#1)")
	ASSERT(!scmp(r1->publisher, "Hachette"), "record data should match expected (#2)")
	ASSERT(scnt(r1->subject)==r1->lengths[3], "r->lengths[] should agree with reality")

	free(r);
	free(r1);
	R0;
}

I main() { R idx_test(); }
#endif

//:~