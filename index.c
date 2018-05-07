#include <stdlib.h>
#include <string.h>
#include "books.h"
#include "dynarray.h"
#include "binsearch.h"

#define BUFSIZE 1024			//< read BUFSIZE records at a time

typedef struct idx {
	UJ book_id;
	UJ pos;
} Idx;

Arr *book_index;

C index_cmp(V*a, V*b, size_t t){
	UJ x = ((Idx*)a)->book_id;
	UJ y = ((Idx*)b)->book_id;
	J r = x-y;
	//O("index_cmp %lu %lu r=%ld\n", x, y, r);
	R !r?r:r<0?-1:1;
}

J rec_get_pos(Arr* idx, UJ book_id) {
	R binfn(idx->data, &book_id, Idx, idx->used, (BIN_CMP_FN)&index_cmp);
}

V rec_print(Book *b) {
	O("record: id=(%lu) pages=(%d) year=(%d) title=(%s) author=(%s)\n", b->book_id, b->pages, b->year, b->title, b->author);
}

J rec_get(Book *dest, UJ book_id) {
	J pos = rec_get_pos(book_index, book_id);
	//O("rec_get pos=%ld\n", pos);
	if(pos < 0) R pos;
	J offset = pos * SZ(Book);
	FILE *db = fopen("books.dat", "r");
	fseek(db, offset, SEEK_SET);
	fread(dest, SZ(Book), 1, db);
	fclose(db);
	R pos;
}

V rec_build_idx(S fname) {
	FILE *in = fopen(fname, "r");

	book_index = arr_init(BUFSIZE, Idx);

	UJ rcnt, pos, last_book_id;
	Book buf[BUFSIZE];

	while((rcnt = fread(buf, SZ(Book), BUFSIZE, in))) {
		Idx entry;
		DO(rcnt,
			Book *b = &buf[i];
			if (b->book_id < last_book_id)
				O("WARN: unsorted book_id=(%lu) at pos=(%lu)\n", b->book_id, pos);
			last_book_id = entry.book_id = b->book_id;
			entry.pos = pos++;
			arr_add(book_index, entry);
		)
	}

	fclose(in);
	O("built record index: %lu entries\n", book_index->used);
}

V rec_destroy_idx() {
	arr_free(book_index);
}

I main() {
	rec_build_idx("books.dat");
	Book b;
	J res = rec_get(&b, 666);
	rec_print(&b);
}