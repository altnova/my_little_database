#define _GNU_SOURCE

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define C char
#define S char*
#define I int
#define H short
#define UJ unsigned long
#define R return
#define O printf
#define SUB_LEN 148
#define NAME_LEN 50
#define SZ sizeof

typedef struct books {
	UJ book_id;
	I pages;
	H year;
	C publisher[NAME_LEN];
	C title[NAME_LEN];
	C author[NAME_LEN];
	C subject[SUB_LEN];
} Book;

enum txt_fields { fld_publisher, fld_title, fld_author, fld_subject };

int txt_field_offsets[] = {
	offsetof(Book, publisher), offsetof(Book, title), offsetof(Book, author), offsetof(Book, subject)
};

int rec_search_txt_field(void *rec, int fld, S needle) {
	S haystack = (S)rec+txt_field_offsets[fld];
	R !!strcasestr(haystack, needle);
}

int main() {
	Book r;

	O("%d %d %d %d %d %d\n", SZ(UJ), SZ(I), SZ(I), SZ(C), SZ(H), NAME_LEN*5 + SUB_LEN);
	O("%d %d\n", SZ(UJ) + SZ(I) + SZ(H) + NAME_LEN*3 + SUB_LEN, SZ(Book));

	strcpy( r.publisher, "Hachette" );
	strcpy( r.title, "IDIOT" );	
	strcpy( r.author, "Dostoyevsky" );

	O("publisher match: %d\n", rec_search_txt_field(&r, fld_publisher, "ette"));
	O("title match: %d\n", rec_search_txt_field(&r, fld_title, "Idiot"));
	O("surname name: %d\n", rec_search_txt_field(&r, fld_author, "dosto"));

	R 0;
}