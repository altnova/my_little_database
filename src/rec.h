//!\file rec.h \brief database record api

typedef UJ(*REC_FN)(Rec r);
typedef  V(*REC_SET_FN)(Rec r, I fld_id, V*val);

//! network client only uses rec_set()
#ifndef NET_CLIENT

#include "alg/bin.h"
#include "utl/fio.h"

//! basic operations on records
//! \return NIL in case of failure
ext UJ rec_get(Rec dest, ID rec_id);

ext UJ rec_create(Rec src);
ext UJ rec_update(Rec src);
ext UJ rec_delete(ID rec_id);

//! case-insensitive search in field
ext C rec_search(V* rec, I fld, S needle);

//! short format for debugging
ext V rec_print_dbg(Rec b);

//! useful for debugging
ext UJ rec_get_db_pos(ID rec_id);
ext UJ rec_get_idx_pos(ID rec_id);

#endif

//! update a field with given value
ext V rec_set(Rec dest, I fld, V* val);


//:~