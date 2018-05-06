#define C char
#define S char*
#define I int
#define H short
#define V void
#define UJ unsigned long
#define R return
#define O printf
#define SZ sizeof
#define BUF 1000000
#define QUO '"'
#define DEL ';'
#define NUL '\0'
#define LF '\n'
#define FLDMAX 2000
#define RECBUFLEN 2048
#define DO(n,x) {long i=0,_i=(n);for(;i<_i;++i){x;}}

typedef struct books {
	UJ book_id;
	H pages;
	H year;
	C publisher[101];
	C title[201];
	C author[51];
	C subject[2001];
} Book;

enum rec_fields { fld_pages, fld_year, fld_publisher, fld_title, fld_author, fld_subject };

I csv_max_field_widths[] = { 4, 4, 100, 200, 50, 2000 };

I rec_field_offsets[] = {
	offsetof(Book, pages), offsetof(Book, year), offsetof(Book, publisher),
	offsetof(Book, title), offsetof(Book, author), offsetof(Book, subject)
};


//:~