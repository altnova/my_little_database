//!\file idx.h \brief index file api

ext C db_file[], idx_file[];

//! expect bad stuff to happen
//! if you forget to call db_init() first
ext UJ    db_init(S db_file, S idx_file);

ext ID    next_id();

ext UJ    idx_add(ID rec_id, UJ pos);
ext UJ    idx_shift(UJ pos);
ext UJ    idx_update_pos(ID rec_id, UJ new_pos);
ext Pair* idx_data();
ext Pair* idx_get_entry(UJ idx_pos);
ext UJ    idx_size();

ext C     cmp_binsearch(V* a, V* b, sz t);

ext V     db_close(); //< don't forget

//:~