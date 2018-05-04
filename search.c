#define _GNU_SOURCE

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define C char
#define S char*
#define I int
#define H short
#define R return
#define O printf
#define SUB_LEN 140
#define NAME_LEN 50
#define SZ sizeof

typedef struct books {
	I book_id;
	C deleted;
	H year;
	C publisher[NAME_LEN];
	I pages;
	C title[NAME_LEN];
	C name[NAME_LEN];
	C surname[NAME_LEN];
	C patronymic[NAME_LEN];
	C subject[SUB_LEN];
} Book;

enum txt_fields { fld_publisher, fld_title, fld_name, fld_surname, fld_patronymic, fld_subject };

int txt_field_offsets[] = {
	offsetof(Book, publisher), offsetof(Book, title), offsetof(Book, name),
	offsetof(Book, surname), offsetof(Book, patronymic), offsetof(Book, subject)
};

int rec_search_txt_field(void *rec, int fld, S needle) {
	S haystack = (S)rec+txt_field_offsets[fld];
	R !!strcasestr(haystack, needle);
}

int main() {
	Book r;
	S query = "idiot";

	strcpy( r.publisher, "Hachette" );
	strcpy( r.title, "IDIOT" );	
	strcpy( r.surname, "Dostoyevsky" );

	O("publisher match: %d\n", rec_search_txt_field(&r, fld_publisher, "ette"));
	O("title match: %d\n", rec_search_txt_field(&r, fld_title, "Idiot"));
	O("surname name: %d\n", rec_search_txt_field(&r, fld_surname, "dosto"));

	R 0;
}