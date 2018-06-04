//!\file cfg.c \brief database settings

#pragma once

#include <stddef.h>
#include "___.h"
#define TEST_MODE

//! L_FATAL|L_WARN|L_INFO|L_TEST|L_DEBUG|L_TRACE
#ifdef TEST_MODE
#define LOGLEVEL L_TEST
#define CSV_FILE "csv/books.csv"
#else
#define LOGLEVEL L_INFO	//< set to WARN in prod builds
#define CSV_FILE "csv/books.csv"
#endif

#define DAT_FILE "dat/books.dat"
#define IDX_FILE "dat/books.idx"

//! csv parser config
#define COLS 6				 /*< csv column count */
#define CSV_READ_BUF 2000000 /*< csv read buffer */
#define QUO '"'				 /*< quote char */
#define DELIM ';'			 /*< csv delimiter */
#define NUL '\0'			 /*< null char */
#define LF '\n'				 /*< newline char */
#define FLDMAX 2000			 /*< longest field len */
#define RECBUFLEN 4096		 /*< record r/w buffer */

#define MAX_FNAME_LEN 200	 /*< db and idx files */

//! full-text index
#define FTS_MAX_HITS 15

//! network config
#define NET_STREAM_BUF_SZ 4	/*< stream buffer size for LST and FND */
#define NET_PORT 5000		/*< default tcp port */
#define NET_START_STREAM 0xffffffff /*< uint_max: magic value for stream header packet */

typedef UJ ID;				/*< rec_id type alias */

//! index entry
typedef struct idx_entry {
	ID rec_id;	//< record id
	UJ pos;		//< file position
} Pair;

//! database record
typedef struct record {
   ID rec_id;
   UH pages;
   UH year;
	C publisher[101];
	C title[201];
	C author[51];
	C subject[2001];
   UH lengths[4];
} __attribute__((packed)) pRec;
typedef pRec bufRec[RECBUFLEN];
typedef pRec* Rec;
#define SZ_REC SZ(pRec)

typedef struct db_info {
	UJ total_records;
	UJ total_words;
	UJ total_mem;
} __attribute__((packed)) pDB_INFO;
typedef pDB_INFO* DB_INFO;
#define SZ_DB_INFO SZ(pDB_INFO)

enum rec_fields { fld_pages, fld_year,   fld_publisher,
                  fld_title, fld_author, fld_subject };

ZS rec_field_names[] = {"pages", "year",   "publisher",
                        "title", "author", "subject"};

ZI csv_max_field_widths[] = { 4, 4, 100, 200, 50, 2000 };

ZI rec_field_offsets[] = {
	offsetof(pRec, rec_id),
	offsetof(pRec, pages), offsetof(pRec, year), offsetof(pRec, publisher),
	offsetof(pRec, title), offsetof(pRec, author), offsetof(pRec, subject)
};

#define FTI_FIELD_COUNT 6 //< count of full-text search fields
//! delimiters for text tokenizer
#define FTI_TOKEN_DELIM "\"' %$`^0123456789#@_?~;&/\\,!|+-.:()[]{}<>*=\"\t\n\r" 

enum msg_err_codes{
	ERR_INVALID_RPC_VERSION = 1,
	ERR_MSG_IS_TOO_BIG = 2,
	ERR_NOT_YET_IMPLEMENTED = 3,
	ERR_UNKNOWN_MSG_TYPE = 4,
	ERR_NO_SUCH_RECORD = 5,
	ERR_NOT_SUPPORTED = 6,
	ERR_CMD_FAILED = 7,
	ERR_BAD_RECORD_ID = 8,
	ERR_MSG_WOULD_BLOCK = 9
};

//:~
