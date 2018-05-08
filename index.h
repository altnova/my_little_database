#define BUFSIZE 1024			//< read BUFSIZE records at a time

typedef struct idx {
	UJ book_id;
	UJ pos;
} Idx;

extern J rec_get_pos(Arr* idx, UJ book_id);
extern J rec_get(Book *dest, UJ book_id);
extern V rec_rebuild_idx(S fname);
extern V rec_save_idx(S fname);
extern V rec_load_idx(S fname);
extern UJ next_id();

extern V rec_close_idx();	//< don't forget!

