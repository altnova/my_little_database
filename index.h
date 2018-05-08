#define BUFSIZE 1024			//< read BUFSIZE records at a time

typedef struct idx {
	UJ book_id;
	UJ pos;
} Idx;

extern UJ next_id();

extern UJ rec_get_pos(Arr* idx, UJ book_id);
extern UJ rec_get(Book *dest, UJ book_id);
extern UJ rec_delete(UJ book_id);

extern V idx_rebuild(S fname);
extern V idx_save(S fname);
extern V idx_load(S fname);

extern V idx_close();	//< don't forget!

//:~