#define BUFSIZE 1024			//< read BUFSIZE records at a time
#define MAX_FNAME_LEN 200		//< db and idx files

typedef struct idx {
	UJ book_id;
	UJ pos;
} Idx;

extern C db_file[], idx_file[];
extern Arr *book_index;

extern V db_init(S db_file, S idx_file);

extern UJ next_id();

extern V idx_rebuild();
extern V idx_save();
extern V idx_load();

extern V idx_add(UJ book_id, UJ pos);
extern UJ idx_shift(UJ pos);
extern UJ idx_update_pos(UJ book_id, UJ new_pos);

extern C cmp_binsearch(V*a, V*b, size_t t);

extern V idx_close();	//< don't forget!

//:~