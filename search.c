#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "books.h"

FILE * outfile;

I recbufpos = 0;
UJ last_id = 0;
Book recbuf[RECBUFLEN];

I rec_search_txt_field(V *rec, I fld, S needle) {
	S haystack = (S)rec+rec_field_offsets[fld];
	R !!strcasestr(haystack, needle);
}

V rec_print(Book *b) {
	//O("%lu %d %d %s %s\n", b->book_id, b->pages, b->year, b->title, b->author);
}

V recbuf_flush() {
	fwrite(recbuf, SZ(Book), recbufpos, outfile);
	O("[+] %d records\n", recbufpos);
	recbufpos = 0;
}

UJ next_id() {
	last_id++;
	return last_id;
}

void add_field(I fld, S val) {
	I offset = rec_field_offsets[fld];
	V *r = (V*)&recbuf[recbufpos];
	V *f = r+offset;

	if (fld < 2) {
		H i = (H)atoi(val);
		memcpy(f,&i, SZ(H));
	} else
		strcpy(f, val);

	if (fld == 5) {
		UJ id = next_id();
		memcpy(r, &id, SZ(UJ));
		rec_print(&recbuf[recbufpos++]);
	}

	if (recbufpos == RECBUFLEN)
		recbuf_flush();
}

void db_load(S fname) {
	FILE *csv = fopen(fname, "r+");

	if (csv == NULL)
        exit(EXIT_FAILURE);

	I bytesRead, fld=-1, fldpos=0;
	C buf[BUF], is_line_end, is_fld_end, in_skip, in_field, in_quotes, curr, prev, fldbuf[FLDMAX+1];
	
	while ((bytesRead = fread(buf, 1, BUF, csv)) > 0) {
		DO(bytesRead,
			curr = buf[i];				//< current byte
			is_line_end = curr == LF; 
			is_fld_end = is_line_end
				|| in_quotes && prev == QUO && curr == DEL
				|| !in_quotes && curr == DEL; //< field end flag

			//O("fld=%d maxw=%d\n", fld+1, csv_max_field_widths[fld+1]);
			if (fldpos == csv_max_field_widths[fld+1]) //< field is longer than max length, enter skip state
				in_skip=1;

			if (in_skip) {				//< while in skip state
				if (!is_fld_end) {		//< field continues, keep skipping
					prev = curr;
					continue;
				} else {				//< reached field end, flush first FLDMAX chars
					fld++;
					prev = NUL;			//< if long field was quoted, discard closing quote
					goto FLUSH;
				}
			}

			if (is_line_end) {			//< reached line end
				fld++;
				FLUSH:					//< catch-all field flush routine
				prev = fldbuf[fldpos-(prev==QUO)] = NUL; //< insert string terminator

				//if (fld < 2)
				//	O("%d %d %s\n", fld, (I)strlen(fldbuf), fldbuf); //< debug output

				add_field(fld, fldbuf);

				in_skip = in_field = in_quotes = fldpos = 0; //< reset states
				if (curr == LF) fld = -1; //< reset field count
				continue;
			}

			if (!in_field) {			//< reached field start
				in_field=1;				//< enter in-field state
				if (curr == QUO)		//< if first char is quote...
					in_quotes=1;		//< ..enter quoted state
				else					
					goto STORECHAR;
			} else {
				if (in_quotes && prev == QUO && curr == QUO) { //< escape sequence
					prev = NUL;			//< discard
					continue;
				}
				if (is_fld_end) {		//< reached field end
					fld++;
					goto FLUSH;
				}
				STORECHAR:
				fldbuf[fldpos++] = prev = curr;	//< copy current char into field bufer
			}
		)
	}

	fclose(csv);
}

int main() {
	Book r;

	outfile = fopen("books.dat", "w+");

	if (outfile == NULL)
        exit(EXIT_FAILURE);

	db_load("books.csv");
	recbuf_flush();

	fclose(outfile);

	exit(0);

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