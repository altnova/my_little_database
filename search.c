#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "books.h"

I rec_search_txt_field(V *rec, I fld, S needle) {
	S haystack = (S)rec+rec_field_offsets[fld];
	R !!strcasestr(haystack, needle);
}

int main() {
	Book r;

	//O("%d %d %d %d %d\n", SZ(UJ), SZ(I), SZ(I), SZ(C), SZ(H));
	//O("%d %d\n", SZ(UJ) + SZ(I) + SZ(H) + 101+201+51+2001, SZ(Book));

	strcpy( r.publisher, "Hachette" );
	strcpy( r.title, "IDIOT" );	
	strcpy( r.author, "Dostoyevsky" );

	O("publisher match: %d\n", rec_search_txt_field(&r, fld_publisher, "ette"));
	O("title match: %d\n", rec_search_txt_field(&r, fld_title, "Idiot"));
	O("surname name: %d\n", rec_search_txt_field(&r, fld_author, "dosto"));

	R 0;
}