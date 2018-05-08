extern UJ rec_get(Book *dest, UJ book_id);
extern UJ rec_create(Book *b);
extern UJ rec_update(Book *b);
extern UJ rec_delete(UJ book_id);
extern V  rec_set(V*b, I fld, V* val);

extern V rec_print_dbg(Book *b);

extern UJ rec_get_db_pos(UJ book_id);
extern UJ rec_get_idx_pos(Arr* idx, UJ book_id);
