#include <stdlib.h>
#include <string.h>
#include "books.h"
#include "dynarray.h"
#include "binsearch.h"
#include "index.h"

Z Arr *book_index = 0;

Z UJ fsize(FILE *fp){
	UJ prev = ftell(fp);
	fseek(fp, 0L, SEEK_END);
	UJ sz = ftell(fp);
	fseek(fp, prev, SEEK_SET);
	R sz;
}

Z J _c(const V*a, const V*b) {
	UJ x = ((Idx*)a)->book_id;
	UJ y = ((Idx*)b)->book_id;
	R (x-y);
}

Z C index_cmp(V*a, V*b, size_t t){
	J r = _c(a,b);
	//O("index_cmp %lu %lu r=%ld\n", x, y, r);
	R !r?r:r<0?-1:1;
}

Z I qsort_cmp(const V*a, const V*b) {
	J r = _c(a,b);
	//O("cmp %lu %lu", x, y);
	R r;
}

J rec_get_pos(Arr* idx, UJ book_id) {
	R binfn(idx->data, &book_id, Idx, idx->used, (BIN_CMP_FN)&index_cmp);
}

Z V rec_print(Book *b) {
	O("record: id=(%lu) pages=(%d) year=(%d) title=(%s) author=(%s)\n", b->book_id, b->pages, b->year, b->title, b->author);
}

J rec_get(Book *dest, UJ book_id) {
	J pos = rec_get_pos(book_index, book_id);
	//O("rec_get pos=%ld\n", pos);
	if(pos<0)R pos;
	J offset = pos * SZ(Book);
	FILE *db = fopen("books.dat", "r");
	fseek(db, offset, SEEK_SET);
	fread(dest, SZ(Book), 1, db);
	fclose(db);
	R pos;
}

Z V rec_print_idx(UJ head) {
	Idx e;
	DO(head?head:book_index->used,
		e = arr_at(book_index, i, Idx);
		O("idx %lu %lu\n", e.book_id, e.pos);
	)
}

V rec_rebuild_idx(S fname) {
	FILE *in = fopen(fname, "r");

	book_index = arr_init(BUFSIZE, Idx);

	UJ rcnt, pos, last_book_id;
	Book buf[BUFSIZE];

	while((rcnt = fread(buf, SZ(Book), BUFSIZE, in))) {
		Idx entry;
		DO(rcnt,
			Book *b = &buf[i];
			//if (b->book_id < last_book_id)
			//	O("WARN: unsorted book_id=(%lu) at pos=(%lu)\n", b->book_id, pos);
			last_book_id = entry.book_id = b->book_id;
			entry.pos = pos++;
			arr_add(book_index, entry);
		)
	}

	fclose(in);
	O("built record index: %lu entries\n", book_index->used);

	qsort(book_index->data, book_index->used, SZ(Idx), qsort_cmp);

	Idx last = arr_last(book_index,Idx);

	O("index sorted, last id=%ld\n", last.book_id);

	book_index->hdr = last.book_id; 	//< use Arr header field for last_id
}

V rec_destroy_idx() {
	arr_free(book_index);
}

V rec_save_idx(S fname) {
	FILE *out = fopen(fname, "w+");
	fwrite(book_index, SZ(Arr)+book_index->size*SZ(Idx), 1, out);
	J size = fsize(out);
	fclose(out);
	O("index saved, %lu bytes\n", size);
}

V rec_load_idx(S fname) {
	FILE *in = fopen(fname, "r");
	rec_destroy_idx();
	J size = fsize(in);
	Arr**idx = &book_index;
	*idx = arr_init((size-SZ(Arr))/SZ(Idx), Idx);
	fread(book_index, size, 1, in);
	fclose(in);
	O("loaded record index: %lu bytes, %lu entries, capacity=%lu, last_id=%lu\n", size, book_index->used, book_index->size, book_index->hdr);
	//rec_print_idx(100);
}

UJ next_id() {
	UJ id = ++book_index->hdr;
	FILE *out = fopen("books.idx", "r+");
	fseek(out, 0L, SEEK_SET);
	fwrite(&id, SZ(UJ), 1, out);
	fclose(out);
	return id;
}

V rec_peek(UJ book_id){
	Book b;
	J res = rec_get(&b, book_id);
	rec_print(&b);
}

I main() {
	rec_rebuild_idx("books.dat");
	rec_save_idx("books.idx");
	rec_peek(66666);

	rec_load_idx("books.idx");
	rec_peek(66666);

	DO(3, next_id())

	rec_load_idx("books.idx");
	rec_peek(66666);

	rec_destroy_idx();
}