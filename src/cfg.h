//! application-specific settings

//#define TEST_MODE

#ifdef TEST_MODE
#define LOGLEVEL L_TEST
#define CSV_FILE "csv/sample.csv"
#else
//! debug level (L_WARN|L_INFO|L_TEST|L_DEBUG|L_TRACE)
#define LOGLEVEL L_INFO		//< min verbosity
#define CSV_FILE "csv/books.csv"
#endif

#define DAT_FILE "dat/books.dat"
#define IDX_FILE "dat/books.idx"

//! csv parser config
#define COLS 6				//< csv column count
#define BUF 2000000			//< csv read buffer
#define QUO '"'				//< quote char
#define DELIM ';'			//< csv delimiter
#define NUL '\0'			//< null char
#define LF '\n'				//< newline char
#define FLDMAX 2000			//< longest field len
#define RECBUFLEN 4048		//< db write buffer
#define CSVDEBUG 0			//< debug output

#define MAX_FNAME_LEN 200	//< db and idx files

typedef UJ ID;				//< rec_id type alias

typedef struct idx_entry {
	ID rec_id;
	UJ pos;
} Pair;

typedef struct record {
	ID rec_id;
	H pages;
	H year;
	C publisher[101];
	C title[201];
	C author[51];
	C subject[2001];
} pRec;

typedef pRec bufRec[RECBUFLEN];
typedef pRec* Rec;

#define SZ_REC SZ(pRec)

enum rec_fields { fld_pages, fld_year, fld_publisher, fld_title, fld_author, fld_subject };

Z I csv_max_field_widths[] = { 4, 4, 100, 200, 50, 2000 };

Z I rec_field_offsets[] = {
	offsetof(pRec, pages), offsetof(pRec, year), offsetof(pRec, publisher),
	offsetof(pRec, title), offsetof(pRec, author), offsetof(pRec, subject)
};
