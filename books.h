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

//:~