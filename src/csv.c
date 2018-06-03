//!\file csv.c \brief fast csv parser

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "___.h"
#include "csv.h"

Z FILE* infile;
Z FILE* outfile;
Z ID last_id = 0;
ZI recbufpos = 0;
Z bufRec recbuf;
Z UJ currline;

ZV rec_print(Rec r){
	LOG("rec_print");
	T(DEBUG, "id=(%lu) pages=(%d) year=(%d) title=(%s) author=(%s)",
		r->rec_id, r->pages, r->year, r->title, r->author);}

ZV recbuf_flush(){
	LOG("recbuf_flush");
	fwrite(recbuf, SZ_REC, recbufpos, outfile);	//< flush current buffer to outfile
	T(DEBUG, "flushed %d records", recbufpos);
	recbufpos=0;} //< rewind buffer

Z ID next_id(){R last_id++;}

ZV csv_add_field(UJ line, I fld, S val){
	LOG("add_field");
	if (fld>=COLS)
		T(WARN, "too many columns, skipping: line=(%lu) fld=(%d) val=(%s)", line, fld, val);

	V*r = (V*)&recbuf[recbufpos];			//< void ptr to current record
	I offset = rec_field_offsets[fld+1];	//< offset of current field (0 is reserved for rec_id)
	V*f = r+offset;							//< void pointer to field

	if (fld<2) {							//< pages and year are shorts
		H i = (H)atoi(val);					//< parse integer
		mcpy(f, &i, SZ(H));					//< populate short field
	} else {								//< all other fields are strings
		scpy(f, val, csv_max_field_widths[fld]);	//< populate string field
		((Rec)r)->lengths[fld-2] = scnt(val);		//< update length
	}

	if (fld==COLS-1) {						//< reached last field
		ID id = next_id();					//< allocate rec_id
		mcpy(r, &id, SZ(ID));				//< populate rec_id
		if(((I)id%5000)==0)
			rec_print(&recbuf[recbufpos]);	//< echo every 5000th record
		recbufpos++;
	}

	if (recbufpos==RECBUFLEN)				//< record buffer is full...
		recbuf_flush();						//< ...flush it to disk.
}

UJ csv_fread(C buf[], UI buflen) {
	R fread(buf, 1, buflen, infile);}

UJ csv_load_stream(S csv_fname, S db_fname) {
	LOG("csv_load_stream");
	xfopen(infile, csv_fname, "r+", NIL);
	xfopen(outfile, db_fname, "w+", NIL);
	CSV_STATE s = csv_stream_init(csv_add_field);
	sz bytes,lines;C buf[10]; //< 10 chars per tick

	W(!s->done){ //< imitate event loop
		bytes = csv_fread(buf,10);
		lines = csv_stream_parse(s,buf,bytes);
		T(TRACE,"streaming csv %lu bytes %lu lines", bytes, lines);
	}

	lines = csv_stream_end(s);
	recbuf_flush();	//< flush remaining buffer to disk
	fclose(infile);
	fclose(outfile);
	R lines;}

CSV_STATE csv_stream_init(CSV_ADD_FIELD field_fn) {
	LOG("csv_stream_init");
	CSV_STATE s = (CSV_STATE)calloc(1,SZ(pCSV_STATE));chk(s,NULL);
	s->currline=1;
	s->fld=-1;s->fldpos=0;
	s->done = s->in_skip = s->in_field = s->in_quotes = 0; //< reset states
	s->field_fn = field_fn;
	R s;}

UJ csv_stream_end(CSV_STATE s) {
	LOG("csv_stream_end");
	T(TEST,"total_bytes=%lu total_records=%lu", s->total_bytes, s->currline-1);
	free(s);
	R s->currline-1;}

UJ csv_stream_parse(CSV_STATE s, C buf[], UI bytes){
	LOG("csv_stream_parse");
	if(!bytes){s->done=1;R0;}
	DO(bytes,
		s->curr = buf[i];					//< current char
		s->is_line_end = s->curr==LF;		//< line end flag
		s->is_fld_end = s->is_line_end
			||s->in_quotes && s->prev==QUO && s->curr==DELIM
			||!s->in_quotes && s->curr==DELIM; //< field end flag
		//O("fld=%d maxw=%d\n", fld+1, csv_max_field_widths[fld+1]);
		if (s->fldpos==csv_max_field_widths[s->fld+1]) //< field is longer than max length, enter skip state
			s->in_skip=1;

		if (s->in_skip){				//< while in skip state
			if (!s->is_fld_end){		//< field continues, keep skipping
				s->prev = s->curr;
				continue;
			} else {					//< reached field end, flush first FLDMAX chars
				s->fld++;
				s->prev = NUL;			//< if long field was quoted, discard closing quote
				goto FLUSH;
			}
		}

		if (s->is_line_end){			//< reached line end
			s->fld++;
			FLUSH:						//< catch-all field flush routine
			s->prev = s->fldbuf[s->fldpos-(s->prev==QUO)] = NUL; //< terminate string
			T(TRACE, "field fld=%d len=%d buf=%s",
				s->fld, (I)scnt(s->fldbuf), s->fldbuf);

			s->field_fn(s->currline, s->fld, s->fldbuf);

			s->in_skip = s->in_field = s->in_quotes = s->fldpos = 0; //< reset states
			if (s->is_line_end) {
				s->fld = -1; 			//< reset field count
				s->currline++;			//< increment line counter
			}
			continue;
		}

		if (!s->in_field){			//< reached field start
			s->in_field = 1;			//< enter in-field state
			if (s->curr==QUO)			//< if first char is quote...
				s->in_quotes = 1;		//< ..enter quoted state
			else					
				goto STORECHAR;
		} else {
			if (s->in_quotes && s->prev==QUO && s->curr==QUO) { //< escape sequence
				s->prev = NUL;			//< discard
				continue;
			}
			if (s->is_fld_end) {		//< reached field end
				s->fld++;
				goto FLUSH;
			}
			STORECHAR:
			s->fldbuf[s->fldpos++] = s->prev = s->curr;	//< copy current char into field buffer
		}
	)
	s->total_bytes += bytes;
	R s->currline-1;} //< lines parsed


/*! blocking version
UJ csv_load_file(S csv_fname, S db_fname) {
	LOG("csv_load_file");
	xfopen(infile, csv_fname, "r+", NIL);
	xfopen(outfile, db_fname, "w+", NIL);

	UJ cnt = csv_parse(csv_fread, csv_add_field);
	recbuf_flush();	//< flush remaining buffer to disk

	fclose(infile);
	fclose(outfile);
	R cnt;}
UJ csv_parse(CSV_INPUT_STREAM read_fn, CSV_ADD_FIELD field_fn){
	LOG("csv_load");
	UJ currline = 1;
	I bytesRead, fld=-1, fldpos=0;
	C buf[CSV_READ_BUF], is_line_end, is_fld_end, in_skip,
	in_field, in_quotes, curr, prev, fldbuf[FLDMAX+1];
	in_skip = in_field = in_quotes = 0; //< reset states
	
	W((bytesRead = read_fn(buf,CSV_READ_BUF)) > 0) {
		DO(bytesRead,
			curr = buf[i];				//< current byte
			is_line_end = curr==LF;		//< line end flag
			is_fld_end = is_line_end
				||in_quotes && prev==QUO && curr==DELIM
				||!in_quotes && curr==DELIM; //< field end flag
			//O("fld=%d maxw=%d\n", fld+1, csv_max_field_widths[fld+1]);
			if (fldpos==csv_max_field_widths[fld+1]) //< field is longer than max length, enter skip state
				in_skip=1;

			if (in_skip){				//< while in skip state
				if (!is_fld_end){		//< field continues, keep skipping
					prev = curr;
					continue;
				} else {				//< reached field end, flush first FLDMAX chars
					fld++;
					prev = NUL;			//< if long field was quoted, discard closing quote
					goto FLUSH;
				}
			}

			if (is_line_end){			//< reached line end
				fld++;
				FLUSH:					//< catch-all field flush routine
				prev = fldbuf[fldpos-(prev==QUO)] = NUL; //< terminate string
				T(TRACE, "field fld=%d len=%d buf=%s", fld, (I)scnt(fldbuf), fldbuf);

				field_fn(currline, fld, fldbuf);

				in_skip = in_field = in_quotes = fldpos = 0; //< reset states
				if (is_line_end) {
					fld = -1; 			//< reset field count
					currline++;			//< increment line counter
				}
				continue;
			}

			if (!in_field){			//< reached field start
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
	R currline-1;} //< lines parsed
*/

UJ csv_init(){
	LOG("csv_init");
	R0;}

V csv_close(){
	LOG("csv_close");
	T(TRACE, "csv parser is shut down");}

#ifdef RUN_TESTS_CSV

Z UJ csv_test_file(S csv_file, S db_file) {
	LOG("csv_test");
	X(csv_init(),T(WARN, "csv_init reports error"),NIL)
	R csv_load_stream(csv_file, db_file);}

I main(I argc, S*argv){
	LOG("csv_main");
	UJ res;
	ASSERT(argc==4, "should have 3 arguments")
	ASSERT(fexist(argv[1]), "source csv file should exist")
	UJ expected = atoi(argv[3]);
	ASSERT(expected>0, "3rd argument should be a number greater than 0")

	res = csv_test_file(argv[1], argv[2]);
	ASSERT(res!=NIL, "csv parser should complete normally")
	ASSERT(res==expected, "imported record count should match expected value")

	ASSERT(res==expected, argv[2])
	R0;
}
#endif

//:~
