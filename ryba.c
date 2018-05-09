#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "src/cfg.h"
#include "src/___.h"
#define VER "1.0.0"
#define MI(i,label) O("\t%d. %s", i, label);
#define NL() O("\n");

pRec  b1, b2;

V recprint_compressed(Rec b);
V recprint_formated(Rec b);

/////////////////////////////////////////////////////////////////////////////////////////
//										PLUGS										   //
/////////////////////////////////////////////////////////////////////////////////////////
I rec_delete(UJ num)
{ R (num < 0 || num > 1) ? 1 : 0; }


I rec_add()
{ O("rec_add\n"); R 1;}

V rec_search(I fld, S string)
{
	recprint_compressed(&b1);
	recprint_compressed(&b2);
}

V displayall(I fld)
{
	recprint_compressed(&b1);
	recprint_compressed(&b2);
}

UJ next_id()
{	R 2;}

Rec rec_get(UJ id) 
{	R &b1;}

I rec_display_1(UJ id)
{
	if (id > 1)
		R 0;
	recprint_compressed(&b1);
	R 1;
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

V csv_init(S filename) 
{

}

/////////////////////////////////////////////////////////////////////////////////////////
//									PRINTS											   //
/////////////////////////////////////////////////////////////////////////////////////////

I input_number(UJ *num, S prompt, S errmsg){
    C line[4096];
    while(O("%s: ", prompt) > 0 && fgets(line, SZ(line), stdin) != 0)
        if(sscanf(line, "%d", num) == 1)R 0;else O("%s\n", errmsg);
    R EOF;
}

V recprint_compressed(Rec b)
{
	O("\t(%d)\t%s\t\t%s\t%d\t%s\t%dpp\n", b->rec_id, b->title, b->author, b->year, b->publisher, b->pages);
}

V recprint_formated(Rec b)
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

V get_num(UJ *num, S buf)
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
V get_line(S buf, I length)
{
	C c;
	I i;
	for (i = 0; c != '\n' && i < length && !feof(stdin); i++) {
		scanf("%c", &c);
		// printf("!%d!", c);
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
I scr_deleterec_3(UJ *num)
{
	C buf[15];
	NL()
	O("\nDelete record\n");
	O("\t-------------\n");
	NL();
 	O("enter book ID or press any letter to go back to main menu\n");
 	get_line(buf, 15);
 	get_num(num, buf);

 	if (*num == -1) 
 		R 0;
 
 	if (rec_display_1(*num)) {
 		if (get_yn("this one?")) {
 			rec_delete(*num);
 			O("record ID %lu deleted\n", *num);
 		}
 	}
 	else 
 		O("ERROR: no such record\n");

	R get_yn("delete another one?");
}


I scr_displayrec_5(UJ *num)
{
	C buf[20];
	NL()
	O("\tDisplay record\n");
	O("\t-------------\n");
	NL();	
	O("enter book ID or press any letter to go back to main menu  ");
 	get_line(buf, 20);
 	get_num(num, buf);

 	if (*num == -1) 
 		R 0;

 	if 		(rec_display_1(*num));
 	else	O("ERROR: no such record\n");
 	R get_yn("display another record?");
}



I scr_displayall_6(UJ *num)
{
	NL()
	O("\tDisplay all records\n");
	O("\t-------------\n");
	NL();
	MI(1, "Sorted by Publisher\n")
	MI(2, "Sorted by Year\n")
	MI(3, "By Title\n")
	MI(4, "By Author\n")
	NL()
	MI(0, "Main Menu\n")
	NL()

	input(num, 4, "select command:  ");

	if (!*num)
		R 0;

	*num = 	*num == 1 	? 	fld_publisher 	:
			*num == 2 	? 	fld_year 		:
			*num == 3	? 	fld_title		:
							fld_author;

	displayall(*num);

	R get_yn("sort by another field?");
}


V scr_dbstat_7()
{
	NL()
	O("\tDatabase status\n");
	O("\t-------------\n");
	NL();	
	db_stat();
	O("press enter to return to main menu  ");
	getchar();
}

V scr_dbvacuum_8()
{
	NL()
	O("\tVacuum database\n");
	O("\t-------------\n");
	NL();	
	db_vacuum();
	O("press enter to return to main menu  ");
	getchar();
}

V scr_csv_9()
{
	C filename[20];
	O("\tCSV Import\n");
	O("\t-------------\n");
	O("please, enter import file:  ");
	get_line(filename, 20);

	while (access(filename, F_OK ) != -1)  {
		O("no such file. enter again: ");
		get_line(filename, 20);
	}
	csv_init(filename);
	O("done\n");

}
