#define BUFSIZE 1024			//< read BUFSIZE records at a time
#define MAX_FNAME_LEN 200		//< db and idx files

typedef struct idx {
	UJ book_id;
	UJ pos;
} Idx;

extern V db_init(S db_file, S idx_file);

extern UJ next_id();

extern UJ rec_get(Book *dest, UJ book_id);
extern UJ rec_create(Book *b);
extern UJ rec_update(Book *b);
extern UJ rec_delete(UJ book_id);

extern V rec_set(V*b, I fld, V* val);

extern V idx_rebuild();
extern V idx_save();
extern V idx_load();

extern V idx_close();	//< don't forget!

//:~