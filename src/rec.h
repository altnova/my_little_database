
extern UJ rec_get(Rec dest, ID rec_id);
extern UJ rec_create(Rec b);
extern UJ rec_update(Rec b);
extern UJ rec_delete(ID rec_id);
extern V  rec_set(V*b, I fld, V*val);
extern C  rec_search(V*rec, I fld, S needle);

extern V rec_print_dbg(Rec b);

extern UJ rec_get_db_pos(ID rec_id);
extern UJ rec_get_idx_pos(ID rec_id);

//:~