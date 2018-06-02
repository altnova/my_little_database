//!\file idx.c \brief index file operations

#include <stdlib.h>
#include <string.h>
#include "___.h"
#include "idx.h"

Z IDX idx;		//< in-memory instance of the index
Z bufRec buf;	//< readbuffer RECBUFLEN records

VEC sort_vectors[FTI_FIELD_COUNT+1]; //< plus rec_id=0

C db_file[MAX_FNAME_LEN+1];
C idx_file[MAX_FNAME_LEN+1];

ZI sort_field;
ZC sort_dir;

ZI  db_mapped=0;
ZV* db_memmap;

Z sz idx_close();

//! current idx size
UJ idx_size() {
	R vec_size(idx->pairs);}

//! returns specific index entry
Pair* idx_get_entry(UJ idx_pos) {
	R vec_at(idx->pairs, idx_pos, Pair);}

//! persist index in a file
//! \return NIL on error, byte size on success
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
	R idx_fsize;}

//! squash index entry at given pos and sync index to disk
//! \return NIL on error, new index size on success
UJ idx_shift(UJ pos) {
	LOG("idx_shift");
	vec_del_at(idx->pairs, pos, 1);
	T(DEBUG, "squashed idx_pos=%lu", pos);
	UJ b_written = idx_save();
	X(b_written==NIL, T(WARN, "idx_save failed"), NIL);
	R idx_size();}

//! returns pointer to indexes' data section
Pair* idx_data() {
	R(Pair*)idx->pairs->data;}

//! create a new db file if not exists \returns number of records
Z UJ db_touch(S fname) {
	LOG("db_touch");
	FILE*f;
	xfopen(f, fname, "a", NIL);
	UJ size = fsize(f)/SZ_REC;
	fclose(f);
	R size;}

//! comparator kernel
ZJ _c(const V*a, const V*b) {
	ID x = ((Pair*)a)->rec_id;
	ID y = ((Pair*)b)->rec_id;
	P(x==y,0)
	R x>y?1:-1;}

//! comparator for binfn()
C cmp_binsearch(V* a, V* b, sz t) {
	LOG("cmp_binsearch");
	J r = _c(a,b);
	T(TRACE, "r=%ld\n", r);
	R !r?r:r<0?-1:1;}

//! comparator for qsort()
ZI cmp_qsort(const V*a, const V*b) {
	LOG("cmp_qsort");
	J r = _c(a,b);
	T(TRACE, "%ld\n", r);
	R(I)r;}

ZV*db_mmap_get_field(const V*a){
	UJ x = *(UJ*)a;
	V*rx = db_memmap + x * SZ_REC;
	R rx+rec_field_offsets[sort_field];}

//! string comparator for db_sort()
ZI db_cmp_str(const V*a, const V*b){
	LOG("db_cmp_str");
	S x = (S)db_mmap_get_field(a);
	S y = (S)db_mmap_get_field(b);
	//T(TEST, "comparing (%s) (%s)\n",x,y);
	R !sort_dir?strcmp(x,y):strcmp(y,x);}

//! string comparator for db_sort()
ZI db_cmp_ushort(const V*a, const V*b){
	LOG("db_cmp_ushort");
	UH x = *(UH*)db_mmap_get_field(a);
	UH y = *(UH*)db_mmap_get_field(b);
	P(x==y,0);
	I r=(x>y)?1:-1;
	//T(TRACE, "comparing (%d) (%d) -> %d\n",x,y,r);
	R !sort_dir?r:-r;}

//! string comparator for db_sort()
ZI db_cmp_ulong(const V*a, const V*b){
	LOG("db_cmp_ulong");
	UJ x = *(UJ*)db_mmap_get_field(a);
	UJ y = *(UJ*)db_mmap_get_field(b);
	//T(TEST, "comparing (%s) (%s)\n",x,y);
	P(x==y,0);
	I r=(x>y)?1:-1;
	R !sort_dir?r:-r;}

//! sort index by rec_id
ZV idx_sort() {
	LOG("idx_sort");
	VEC p = idx->pairs;
	qsort(p->data, p->used, SZ(Pair), cmp_qsort);
	T(DEBUG, "index sorted");}

ZV* db_mmap() {
	LOG("db_mmap");
	P(db_mapped,db_memmap);
	db_memmap=xmmap(db_file);chk(db_memmap,NULL);
	db_mapped=1;R0;}

ZV db_munmap() {
	if(!db_mapped)R;
	xmunmap(db_memmap, idx_dbsize());db_mapped=0;}

//! creates a vector of db_positions according to given order
//! and caches it to disk. if vector already exists, do nothing.
UJ db_sort(I f, C d){
	LOG("db_sort");
	sort_dir = d;
	sort_field = f;
	db_mmap();
	VEC a = sort_vectors[f];
	qsort(a->data, a->used, SZ(UJ), sort_field==0?db_cmp_ulong:(f<3?db_cmp_ushort:db_cmp_str));
	db_munmap();
	R0;}

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
	TEND();}

//! linear scan
UJ idx_each(IDX_EACH fn, V*arg, UI batch_size) {
	LOG("idx_each")
	FILE*in;
	X(batch_size>RECBUFLEN,T(WARN,"requested batch size is larger than internal read buffer"),NIL)
	xfopen(in, db_file, "r", NIL);
	UJ rcnt, pos = 0;
	W((rcnt = fread(buf, SZ_REC, RECBUFLEN, in))) {
		T(TRACE, "read %lu records, RECBUFLEN=%d", rcnt, RECBUFLEN);
		Rec b;I batches=rcnt/batch_size;
		T(TRACE, "whole batches: %d", batches);
		DO(batches,
			T(TRACE, "batch offset=%d", i*batch_size);
			b = &buf[i*batch_size];
			fn(b, arg, pos, batch_size);pos+=batch_size;)
		I tail=rcnt%batch_size;
		T(TRACE, "batch tail %d records", tail);
		if(tail){fn(b+batch_size, arg, pos, tail);pos+=tail;}
	}
	fclose(in);
	R pos;}

UJ idx_page(PAGE_EACH fn, V*arg, I page, I page_sz, I sort_fld, C sort_dir){
	LOG("idx_page");
	db_mmap(); // ensure db is mapped
	VEC sv = sort_vectors[sort_fld];
	UI pos = page * page_sz;I j;
	V*a = (V*)sv->data;sz recs = MIN(vec_size(sv)-pos,page_sz);
	//UJ*a = (UJ*)sv->data;sz recs = MIN(vec_size(sv)-pos,page_sz);
	Rec out[recs]; //< array of pointers to mmap
	UI start = !sort_dir?pos:(vec_size(sv)-pos);
	DO(recs,j=!sort_dir?(start+i):(start-i);
		UJ file_pos = *(UJ*)(a + j * SZ(UJ));
		//UJ file_pos = a[j];
		Rec rptr = (Rec)(db_memmap + file_pos * SZ_REC);
		out[i]=rptr;
		T(DEBUG,"page %d: %d:%lu file_pos=%lu -> rec_id=%lu",page,j,i,file_pos,out[i]->rec_id);
	)
	fn(out, recs, arg);
	R recs;}

UJ idx_csv_batch(Rec ptrs[], UI ptr_cnt, V*arg) {
	LOG("idx_csv_batch");
	I fd = *(I*)arg;
	//C fldbuf[];
	T(TEST,"csv tick %d", ptr_cnt);
	DO(ptr_cnt,
		Rec r = ptrs[i];
		DO(FTI_FIELD_COUNT,{

		})
	)
	R0;}

UJ idx_csv_export(I sort_fld, C sort_dir, I fd) {
	LOG("idx_csv_export");
	UI page_size = RECBUFLEN/2;
	DO(1+idx_size()/page_size, //< total pages
		idx_page(idx_csv_batch,&fd,i,page_size,sort_fld,sort_dir);)
	R0;
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
	// once sorted, it is easy to recover last_id
	e = *vec_last(idx->pairs,Pair);
	idx->last_id = e.rec_id;

	T(INFO, "rebuilt, entries=%lu, last_id=%lu", idx_size(), idx->last_id);
	R idx_size();}

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

	R idx_size();}

//! sync index header to disk
UJ idx_update_hdr() {
	LOG("idx_update_hdr");
	FILE*out;
	xfopen(out, idx_file, "r+", NIL);
	fwrite(idx, SZ_IDX, 1, out);
	fclose(out);
	T(DEBUG, "idx header updated");
	R0;}

//! get next available id and store it on disk
UJ next_id() {
	ID id = ++idx->last_id;
	idx_update_hdr();
	R id;}

Z UJ last_id() {
	R idx->last_id;}

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
	R new_pos;}

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
	R new_fsize;}

//! perform sample index lookup
Z UJ idx_peek(ID rec_id){
	LOG("idx_peek");
	Rec b;
	UJ pos = rec_get(b, rec_id);
	X(pos==NIL, T(WARN,"rec_get failed"), NIL); //< no such record
	T(TRACE, "rec_id=%lu pos=%ld", rec_id, pos);
	rec_print_dbg(b);
	R pos;}

sz idx_dbsize() {
	LOG("idx_dbsize");
	FILE*db;
	xfopen(db,db_file,"r",NIL);
	sz db_sz = fsize(db);
	fclose(db);
	R db_sz;}

sz idx_fsize() {
	LOG("idx_fsize");
	FILE*idxh;
	xfopen(idxh,idx_file,"r",1);
	sz fsz = fsize(idxh);
	fclose(idxh);
	R fsz;}

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
	R0;}

//! create if missing, truncate if exist
ZV idx_reset_sort_vectors() {
	LOG("idx_reset_sort_vectors");
	mem_reset("sort_vectors");
	DO(FTI_FIELD_COUNT+1, //< init sort vectors
		if(!sort_vectors[i])
			sort_vectors[i] = vec_init(50,UJ);
		vec_clear(sort_vectors[i]);
		VEC v = sort_vectors[i];
		DO(idx_size(), vec_add(v, i)); //< prime with unsorted
		vec_compact(&sort_vectors[i]);
		mem_inc("sort_vectors", vec_mem(sort_vectors[i]));
	)}

ZV idx_sort_all_vectors() {
	DO(FTI_FIELD_COUNT+1, db_sort(i,0))}

ZV idx_destroy_sort_vectors() {
	DO(FTI_FIELD_COUNT+1,
		if(!sort_vectors[i])continue;
		mem_dec("sort_vectors", vec_destroy(sort_vectors[i]));
		sort_vectors[i]=NULL;
	)}

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

	idx_reset_sort_vectors();
	if(idx_count>0)
		idx_sort_all_vectors();
	T(INFO, "full-text index initilaized, idx_size=%lu", idx_count);
	R idx_size();}

Z sz idx_close() {
	LOG("idx_close");
	idx_destroy_sort_vectors();
 	T(DEBUG, "index closed");
 	R SZ_IDX + vec_mem(idx->pairs);}

UJ db_init(S d, S i) {
	LOG(" db_init");
	scpy(db_file, d, MAX_FNAME_LEN);
	scpy(idx_file, i, MAX_FNAME_LEN);
	idx_open();

	T(DEBUG, "database initialized");
	R SZ_IDX + vec_mem(idx->pairs);}

sz db_close() {
	R idx_close();}

#ifdef RUN_TESTS_IDX

UJ test_walk(Rec r, V*arg, UJ i, I batch_size) {
	LOG("test_walk");
	I*cnt = (I*)arg;
	*cnt+=batch_size;
	//T(TRACE, "rec_id=%lu i=%lu batch=%d", r->rec_id, i, batch_size);
	R0;}

ZI idx_test_core() {
	LOG("idx_test");
	Rec r=malloc(SZ_REC);chk(r,1);
	Rec r1=malloc(SZ_REC);chk(r1,1);
	//db_init(DAT_FILE, IDX_FILE);

	S test_dbfile = "fxt/tempdb.dat";
	S test_idxfile = "fxt/tempdb.idx";

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

	DO(100, rec_create(r))
	ASSERT(idx_size()==103, "bulk add looks good");

	I cnt=0;
	total = idx_each((IDX_EACH)test_walk, &cnt, 1);
	T(TEST,"cnt %d", cnt);
	ASSERT(cnt==103, "idx_each batch=1 works as expected");
	ASSERT(total==103, "idx_each batch=1 works as expected");

	cnt=0;
	total = idx_each((IDX_EACH)test_walk, &cnt, 2);
	T(TEST,"cnt %d", cnt);
	ASSERT(cnt==103, "idx_each batch=2 works as expected");
	ASSERT(total==103, "idx_each batch=2 works as expected");

	cnt=0;
	total = idx_each((IDX_EACH)test_walk, &cnt, 11);
	T(TEST,"cnt %d", cnt);
	ASSERT(cnt==103, "idx_each batch=11 works as expected");
	ASSERT(total==103, "idx_each batch=11 works as expected");

	cnt=0;
	total = idx_each((IDX_EACH)test_walk, &cnt, 100);
	T(TEST,"cnt %d", cnt);
	ASSERT(cnt==103, "idx_each batch=100 works as expected");
	ASSERT(total==103, "idx_each batch=100 works as expected");

	cnt=0;
	total = idx_each((IDX_EACH)test_walk, &cnt, idx_size());
	T(TEST,"cnt %d", cnt);
	ASSERT(cnt==103, "idx_each batch=idx_size() works as expected");
	ASSERT(total==103, "idx_each batch=idx_size() works as expected");

	cnt=0;
	total = idx_each((IDX_EACH)test_walk, &cnt, idx_size()+30);
	T(TEST,"cnt %d", cnt);
	ASSERT(cnt==103, "idx_each batch=idx_size()+n works as expected");
	ASSERT(total==103, "idx_each batch=idx_size()+n works as expected");

	T(TEST,"idx_size %d", idx_size());

	ASSERT(1, "core index functions look good");

	db_close();
	free(r);
	free(r1);
	R0;}

ZI idx_test_sort() {
	DO(FTI_FIELD_COUNT+1,
		I srt_fld = i;
		VEC v = sort_vectors[srt_fld];
		O("  ---------------------------------------------\n");
		O("  orig ");DO(v->used, O("%lu ", *(UJ*)(v->data+i*SZ(UJ))));O("\n");
		db_sort(srt_fld,1);
		O("  desc ");DO(v->used, O("%lu ", *(UJ*)(v->data+i*SZ(UJ))));O("\n");
		db_sort(srt_fld,0);
		O("  xasc ");DO(v->used, O("%lu ", *(UJ*)(v->data+i*SZ(UJ))));O("\n"))
		O("  ---------------------------------------------\n");
	R0;}

UJ idx_test_page_batch(Rec ptrs[], UI ptr_cnt, V*arg) {
	LOG("idx_test_page_batch");
	T(TRACE,"pager tick %d", ptr_cnt);
	DO(ptr_cnt,
		Rec r = ptrs[i];
		T(TEST,"%5lu -> %5d -> %5d -> %.10s -> %.10s -> %.10s -> %.10s",
			r->rec_id, r->pages, r->year, r->publisher,
			r->title, r->author, r->subject);)
	R0;}
UJ idx_test_pagination(UI page_size, I sort_fld, C sort_dir) {
	LOG("idx_test_pagination");
	DO(1+idx_size()/page_size, //< total pages
		T(TRACE,"requesting page %d",i);
		idx_page(idx_test_page_batch,NULL,i,page_size,sort_fld,sort_dir))
	R0;}

I main() {
	LOG("main");
	mem_init();
	//T(TEST,"SZ_REC=%d",SZ_REC);

	idx_test_core(); //< run these first

	db_init("fxt/reference.dat", "fxt/reference.idx");
	
	idx_test_sort();

	DO(FTI_FIELD_COUNT+1,
		O("\n");idx_test_pagination(5,i,0);)//page_size,sort_fld,sort_dir

	//idx_csv_export(0,0,0);

	db_close();
	mem_shutdown();

	ASSERT(1, "file index looks good");
	R0;
}
#endif

//:~