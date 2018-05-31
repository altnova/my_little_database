//! \file csv.h \brief csv parser api

#pragma once

#include "utl/fio.h"

typedef UJ(*CSV_INPUT_STREAM)();
typedef V(*CSV_ADD_FIELD)(UJ line, I fld, S val);

//! load csv file
//! \return #records loaded, NIL on error
extern UJ csv_load(S fname);

//! initialize csv parser
//! \return 0 on success, NIL on error
extern UJ csv_init(S db_fname);

//! release resources
extern V  csv_close();

//:~