//!\file csv.c \brief fast csv parser

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "___.h"
#include "cfg.h"
#include "trc.h"
#include "fio.h"

Z FILE* outfile;
Z ID last_id = 0;
ZI recbufpos = 0;
Z bufRec recbuf;
Z UJ currline;

ZV rec_print(Rec r){
	LOG("rec_print");
	T(TEST, "id=(%lu) pages=(%d) year=(%d) title=(%s) author=(%s)",
		r->rec_id, r->pages, r->year, r->title, r->author);
}

ZV recbuf_flush(){
	LOG("recbuf_flush");
	fwrite(recbuf, SZ_REC, recbufpos, outfile);	//< flush current buffer to outfile
	T(DEBUG, "flushed %d records", recbufpos);
	recbufpos = 0; //< rewind buffer
}

Z ID next_id(){R last_id++;}

ZI add_field(I fld, S val){
	LOG("add_field");
	if (fld>=COLS)
		R T(WARN, "too many columns, skipping: line=(%lu) fld=(%d) val=(%s)", currline, fld, val);

	V*r = (V*)&recbuf[recbufpos];			//< void ptr to current record
	I offset = rec_field_offsets[fld];		//< offset of current field
	V*f = r+offset;							//< void pointer to field

	if (fld<2) {							//< pages and year are shorts
		H i = (H)atoi(val);					//< parse integer
		mcpy(f, &i, SZ(H));					//< populate short field
	} else									//< all other fields are strings
		scpy(f, val, csv_max_field_widths[fld]);	//< populate string field

	if (fld==COLS-1) {						//< reached last field
		ID id = next_id();					//< allocate rec_id
		memcpy(r, &id, SZ(ID));				//< populate rec_id
		if(((I)id%5000)==0)
			rec_print(&recbuf[recbufpos]);	//< debug print every 1000 records
		recbufpos++;
	}

	if (recbufpos==RECBUFLEN)				//< record buffer is full...
		recbuf_flush();						//< ...flush it to disk.
	R0;
}

UJ csv_load(S fname){
	LOG("csv_load");

	FILE* csv;
	xfopen(csv, fname, "r+", NIL);

    currline = 1;
	I bytesRead, fld=-1, fldpos=0;
	C buf[BUF], is_line_end, is_fld_end, in_skip, in_field, in_quotes, curr, prev, fldbuf[FLDMAX+1];
	in_skip = in_field = in_quotes = 0;
	
	W((bytesRead = fread(buf, 1, BUF, csv)) > 0) {
		DO(bytesRead,
			curr = buf[i];				//< current byte
			is_line_end = curr==LF;		//< line end flag
			is_fld_end = is_line_end
				||in_quotes && prev==QUO && curr==DELIM
				||!in_quotes && curr==DELIM; //< field end flag

			//O("fld=%d maxw=%d\n", fld+1, csv_max_field_widths[fld+1]);
			if (fldpos==csv_max_field_widths[fld+1]) //< field is longer than max length, enter skip state
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
				prev = fldbuf[fldpos-(prev==QUO)] = NUL; //< terminate string
				T(TRACE, "field fld=%d len=%d buf=%s", fld, (I)strlen(fldbuf), fldbuf);

				add_field(fld, fldbuf);

				in_skip = in_field = in_quotes = fldpos = 0; //< reset states
				if (is_line_end) {
					fld = -1; 			//< reset field count
					currline++;			//< increment line counter
				}
				continue;
			}

			if (!in_field) {			//< reached field start
				in_field = 1;			//< enter in-field state
				if (curr==QUO)			//< if first char is quote...
					in_quotes = 1;		//< ..enter quoted state
				else					
					goto STORECHAR;
			} else {
				if (in_quotes && prev==QUO && curr==QUO) { //< escape sequence
					prev = NUL;			//< discard
					continue;
				}
				if (is_fld_end) {		//< reached field end
					fld++;
					goto FLUSH;
				}
				STORECHAR:
				fldbuf[fldpos++] = prev = curr;	//< copy current char into field buffer
			}
		)
	}
	fclose(csv);
	recbuf_flush();						//< flush remaining buffer to disk
	R currline-1; //< lines parsed
}

UJ csv_init(S db_fname){
	LOG("csv_init");
	xfopen(outfile, db_fname, "w+", NIL);
	R0;
}

V csv_close(){
	LOG("csv_close");
	fclose(outfile);
	T(TRACE, "csv parser is shut down");
}

#ifdef RUN_TESTS_CSV

Z UJ csv_test(S csv_file, S db_file) {
	LOG("csv_test");
	X(csv_init(db_file),
		T(WARN, "csv_init reports error"), NIL)
	R csv_load(csv_file);
}

I main(I argc, S*argv){
	LOG("csv_main");
	UJ res;
	X((res=csv_test(argv[1], argv[2]))==NIL,
		T(FATAL, "csv parser test failed"), 1)
	T(TEST, "loaded %d records", res);
	R0;
}
#endif

//:~
