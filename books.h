#include <stddef.h>

typedef char C;
typedef char* S;
typedef int I;
typedef short H;
typedef void V;
typedef unsigned long UJ;

#define R return
#define O printf
#define SZ sizeof

#define DO(n,x) {UJ i=0,_i=(n);for(;i<_i;++i){x;}}

#define SW switch
#define CD default
#define CS(n,x)	case n:x;break;

typedef struct books {
	UJ book_id;
	H pages;
	H year;
	C publisher[101];
	C title[201];
	C author[51];
	C subject[2001];
} Book;

// csv parser config
#define COLS 6				//< csv column count
#define BUF 1000000			//< csv read buffer
#define QUO '"'				//< quote char
#define DELIM ';'			//< csv delimiter
#define NUL '\0'			//< null char
#define LF '\n'				//< newline char
#define FLDMAX 2000			//< longest field len
#define RECBUFLEN 2048		//< db write buffer
#define CSVDEBUG 0			//< debug output

enum rec_fields { fld_pages, fld_year, fld_publisher, fld_title, fld_author, fld_subject };

I csv_max_field_widths[] = { 4, 4, 100, 200, 50, 2000 };

I rec_field_offsets[] = {
	offsetof(Book, pages), offsetof(Book, year), offsetof(Book, publisher),
	offsetof(Book, title), offsetof(Book, author), offsetof(Book, subject)
};


//:~