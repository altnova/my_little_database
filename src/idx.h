
extern C db_file[], idx_file[];

//! expect bad stuff to happen
//! if you forget to call db_init() first
extern UJ db_init(S db_file, S idx_file);

extern ID next_id();

extern UJ idx_rebuild();
extern UJ idx_save();
extern UJ idx_load();

extern UJ idx_add(ID rec_id, UJ pos);
extern UJ idx_shift(UJ pos);
extern UJ idx_update_pos(ID rec_id, UJ new_pos);
extern Pair* idx_data();
extern Pair* idx_get_entry(UJ idx_pos);
extern UJ idx_size();

extern C cmp_binsearch(V*a, V*b, size_t t);

//! don't forget to call this one, too
extern V idx_close();

//:~