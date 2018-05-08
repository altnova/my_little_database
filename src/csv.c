#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "___.h"
#include "cfg.h"
#include "trc.h"

V die(S,UJ);

FILE * outfile;

UJ currline;
I recbufpos = 0;
ID last_id = 0;
bufRec recbuf;

V rec_print(Rec b) {
	LOG("rec_print");
	T(TEST, "id=(%lu) pages=(%d) year=(%d) title=(%s) author=(%s)", b->rec_id, b->pages, b->year, b->title, b->author);
}

V recbuf_flush() {
	LOG("recbuf_flush");
	fwrite(recbuf, SZ_REC, recbufpos, outfile);	//< flush current buffer to outfile
	T(INFO, "loaded %d records", recbufpos);
	recbufpos = 0; //< rewind buffer
}

ID next_id() {
	R last_id++;
}

I add_field(I fld, S val) {
	LOG("add_field");
	if (fld >= COLS)
		R T(WARN, "too many columns, skipping: line=(%lu) fld=(%d) val=(%s)", currline, fld, val);

	V *r = (V*)&recbuf[recbufpos];			//< void ptr to current record
	I offset = rec_field_offsets[fld];		//< offset of current field
	V *f = r+offset;						//< void pointer to field

	if (fld < 2) {							//< pages and year are shorts
		H i = (H)atoi(val);					//< parse integer
		memcpy(f, &i, SZ(H));				//< populate short field
	} else									//< all other fields are strings
		strcpy(f, val);						//< populate string field

	if (fld == COLS-1) {					//< reached last field
		ID id = next_id();					//< allocate rec_id
		memcpy(r, &id, SZ(ID));				//< populate rec_id
		rec_print(&recbuf[recbufpos++]);	//< debug print
	}

	if (recbufpos == RECBUFLEN)				//< record buffer is full...
		recbuf_flush();						//< ...flush it to disk.

	R 0;
}

I csv_load(S fname) {
	LOG("csv_load");
	FILE *csv = fopen(fname, "r+");

	if (csv == NULL)
		R T(WARN, "cannot open infile: %s", fname);

    currline = 1;
	I bytesRead, fld=-1, fldpos=0;
	C buf[BUF], is_line_end, is_fld_end, in_skip, in_field, in_quotes, curr, prev, fldbuf[FLDMAX+1];
	
	while((bytesRead = fread(buf, 1, BUF, csv)) > 0) {
		DO(bytesRead,
			curr = buf[i];				//< current byte
			is_line_end = curr == LF; 
			is_fld_end = is_line_end
				|| in_quotes && prev == QUO && curr == DELIM
				|| !in_quotes && curr == DELIM; //< field end flag

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

				if (CSVDEBUG)
					O("field fld=%d len=%d buf=%s\n", fld, (I)strlen(fldbuf), fldbuf);

				add_field(fld, fldbuf);

				in_skip = in_field = in_quotes = fldpos = 0; //< reset states
				if (is_line_end) {
					fld = -1; 			//< reset field count
					currline++;			//< increment line counter
				}
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
	recbuf_flush();						//< flush remaining buffer to disk

	R 0;
}

Z I csv_test() {
	LOG("csv_test");
	outfile = fopen(DAT_FILE, "w+");

	if (outfile == NULL)
		R T(WARN, "cannot open outfile: %s", DAT_FILE);

	I res = csv_load(CSV_FILE);
	
	fclose(outfile);

	R res;
}

I main() { R csv_test(); }

//:~
