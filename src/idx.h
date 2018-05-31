//!\file idx.h \brief index file api

#pragma once

#include "adt/vec.h"
#include "rec.h"
#include "utl/fio.h"
#include "utl/clk.h"

typedef struct idx {
	ID   last_id;
	VEC  pairs;
} pIDX;

#define SZ_IDX SZ(pIDX)
typedef pIDX* IDX;

ext C db_file[], idx_file[];

ext sz    db_init(S db_file, S idx_file);

ext ID    next_id();
ext UJ    idx_add(ID rec_id, UJ pos);
ext UJ    idx_shift(UJ pos);
ext UJ    idx_update_pos(ID rec_id, UJ new_pos);
ext Pair* idx_data();
ext Pair* idx_get_entry(UJ idx_pos);
ext UJ    idx_size();

//! idx_each function interface
typedef UJ(*IDX_EACH)(Rec r, V*arg, UJ i, C is_last);

//! apply fn() to each record in the database
//! \param fn function that takes (Rec r, V* arg, UJ i)
//! \param arg argument to be passed to each fn() call
//! \return 0 on success, NIL on error
ext UJ    idx_each(IDX_EACH fn, V*arg);
ext UJ    idx_page(IDX_EACH fn, V*arg, I page, I page_sz);

ext V     idx_dump(UJ head); // debug
ext C     cmp_binsearch(V*a, V*b, sz t);

ext sz    db_close(); //< don't forget

//:~