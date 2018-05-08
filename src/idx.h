
typedef struct idx {
	UJ rec_id;
	UJ pos;
} Pair;

extern C db_file[], idx_file[];

extern V db_init(S db_file, S idx_file);

extern UJ next_id();

extern V idx_rebuild();
extern V idx_save();
extern V idx_load();

extern V idx_add(UJ rec_id, UJ pos);
extern UJ idx_shift(UJ pos);
extern UJ idx_update_pos(UJ rec_id, UJ new_pos);
extern Pair* idx_data();
extern Pair* idx_get_entry(UJ idx_pos);
extern UJ idx_size();

extern C cmp_binsearch(V*a, V*b, size_t t);

extern V idx_close();	//< don't forget!

//:~