#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "books.h"
#define VER "1.0.0"
#define MI(i,label) O("\t%d. %s", i, label);
#define NL() O("\n");

Book b1, b2;


/////////////////////////////////////////////////////////////////////////////////////////
//									PRINTS											   //
/////////////////////////////////////////////////////////////////////////////////////////

V recprint_compressed(Book *b)
{
	O("\t(%d)\t%s\t\t%s\t%d\t%s\t%dpp\n", b->book_id, b->title, b->author, b->year, b->publisher, b->pages);
}

V recprint_formated(Book *b)
{
	NL()
	MI(1, "TITLE:\t"); 		O("%s\n", b->title);
	MI(2, "AUTHOR:\t"); 	O("%s\n", b->author);
	MI(3, "YEAR:\t"); 		O("%d\n", b->year);
	MI(4, "PUBLISHER:\t"); 	O("%s\n", b->publisher);
	MI(5, "PAGES:\t"); 		O("%d\n", b->pages);
	MI(6, "SUBJECT:\t");	O("%s\n\n", b->subject);
	MI(0, "Done\n");
}

V banner() 
{
	O("\tAmazon Kindle Database v%s\n", VER);
	O("\t_____________________________\n");
}

/////////////////////////////////////////////////////////////////////////////////////////
//										PROCESS										   //
/////////////////////////////////////////////////////////////////////////////////////////


/*	asks user to tap something */
C get_yn(const C *hint) 
{
    C c;
    O("%s [y/n] ", hint);
    c = getchar();
    getchar();
    R c == 'y' ? 1 : 0;
}

V get_num(UJ *num, C buf[])
{
	C c = '1', i = 0;
	for (*num = 0; c != '\n'; i++) {
		c = buf[i];

		if (c == '\n' || c == '\0') 
			break;

		if (c < '0' || c > '9') {
			*num = -1;
			R;
		}

		*num *= 10;
		*num += c - '0';
	}
}

/* 	gets one line from a file */
V get_line(C buf[], I length)
{
	C c;
	I i;
	for (i = 0; c != '\n' && i < length && !feof(stdin); i++) {
		scanf("%c", &c);
		buf[i] = c;
	}
	if (i == length) 
		while (c != '\n' && !feof(stdin))
			scanf("%c", &c);
	
	buf[--i] = '\0';
}


V input(UJ *command, I num, const S request)
{
	C buf[300];
	O(request);
	get_line(buf, 300);
	get_num(command, buf);

	while(*command == -1 || *command > num) {
		O("\x1B[31mERROR:\x1B[0m unknown command\n");
		O(request);
		get_line(buf, 300);
		get_num(command, buf);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//										RYBY										   //
/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////
V csv_9() {}
/////////////////////////////////////////////////////////////////////////////////////////
I rec_delete(UJ num)
{ 
	if (num < 0 || num > 1)
		R 0;
	else 
	 	R 1;
}

I rec_add()
{ O("rec_add\n"); R 1;}

V rec_search(I fld, S string)
{
	recprint_compressed(&b1);
	recprint_compressed(&b2);
}

V rec_sort(I fld)
{
	recprint_compressed(&b1);
	recprint_compressed(&b2);
}

UJ next_id()
{
	R 2;
}

Book* rec_get(UJ id) {

	R &b1;
}



I rec_display_1(UJ id)
{
	if (id > 1)
		R 0;
	recprint_compressed(&b1);
}

I rec_display_fotmated(UJ id)
{
	if (id > 1)
		R 0;
	recprint_formated(rec_get(id));
	R 1;
}


V db_stat()
{
	O("\nData file:\t~/arina/books.dat\t(800 bytes)\nIndex file:\t~/arina/books.idx\t(64 bytes)\n");
	O("Records\t\t2\nDeleted\t\t0\nLast ID:\t1\n\n");
}

V db_vacuum()
{
	O("[OK: Successfuly purged 2 deleted records.]\n\n");
}
