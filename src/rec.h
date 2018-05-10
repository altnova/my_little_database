//!\file rec.h \brief database record api

//! basic CRUD operations on records
//! \return NONE in case of failure
extern UJ rec_get(Rec dest, ID rec_id);
extern UJ rec_create(Rec src);
extern UJ rec_update(Rec src);
extern UJ rec_delete(ID rec_id);

//! update a field with given value
extern V rec_set(Rec dest, I fld, V*val);

//! case-insensitive search in field
extern C rec_search(V*rec, I fld, S needle);

//! short format for debugging
extern V rec_print_dbg(Rec b);

//! useful for debugging
extern UJ rec_get_db_pos(ID rec_id);
extern UJ rec_get_idx_pos(ID rec_id);

//:~