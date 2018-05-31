//! \file csv.h \brief csv parser api

#pragma once

#include "utl/fio.h"

typedef UJ(*CSV_INPUT_STREAM)(C buf[], UI buflen);
typedef V(*CSV_ADD_FIELD)(UJ line, I fld, S val);

typedef struct csv_st {
	UJ currline,total_bytes;
	 I fld,fldpos;
	 C done,buf[CSV_READ_BUF],
	   is_line_end, is_fld_end, in_skip,
	   in_field, in_quotes, curr, prev, fldbuf[FLDMAX+1];
	   CSV_ADD_FIELD field_fn;
} pCSV_STATE;
typedef pCSV_STATE *CSV_STATE;

//! load csv file
//! \return #records loaded, NIL on error
ext UJ csv_load_file(S csv_file, S db_file);

ext UJ csv_parse(CSV_INPUT_STREAM read_fn, CSV_ADD_FIELD field_fn);

ext CSV_STATE csv_stream_init(CSV_ADD_FIELD field_fn);
ext       UJ csv_stream_parse(CSV_STATE s,C buf[],UI bytes);
ext         UJ csv_stream_end(CSV_STATE s);


//! initialize csv parser
//! \return 0 on success, NIL on error
ext UJ csv_init();

//! release resources
ext  V csv_close();

//:~