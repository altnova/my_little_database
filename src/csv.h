//! \file csv.h \brief csv parser api

#pragma once

#include "utl/fio.h"

typedef UJ(*CSV_INPUT_STREAM)(C buf[], UI buflen);
typedef V(*CSV_ADD_FIELD)(UJ line, I fld, S val);

//! load csv file
//! \return #records loaded, NIL on error
ext UJ csv_load_file(S csv_file, S db_file);

ext UJ csv_parse_stream(CSV_INPUT_STREAM read_fn, CSV_ADD_FIELD field_fn);

//! initialize csv parser
//! \return 0 on success, NIL on error
ext UJ csv_init();

//! release resources
ext  V csv_close();

//:~