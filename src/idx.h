//!\file idx.h \brief index file api

ext C db_file[], idx_file[];

//! expect bad stuff to happen
//! if you forget to call db_init() first
ext sz    db_init(S db_file, S idx_file);

ext ID    next_id();

ext UJ    idx_add(ID rec_id, UJ pos);
ext UJ    idx_shift(UJ pos);
ext UJ    idx_update_pos(ID rec_id, UJ new_pos);
ext Pair* idx_data();
ext Pair* idx_get_entry(UJ idx_pos);
ext UJ    idx_size();

//! apply fn() to each record in the database
//! \param fn function that takes (Rec r, V* arg, UJ i)
//! \param arg argument to be passed to each fn() call
//! \return 0 on ok, NIL on error
typedef UJ(*IDX_EACH)(Rec r, V*arg, UJ i); //< idx_each function interface
ext UJ idx_each(IDX_EACH fn, V*arg);
ext UJ idx_page(IDX_EACH fn, V*arg, I page, I page_sz);
ext V idx_dump(UJ head); // debug
ext C     cmp_binsearch(V* a, V* b, sz t);

ext sz    db_close(); //< don't forget

//:~