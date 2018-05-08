#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "___.h"
#include "cfg.h"
#include "trc.h"

#define VER "1.0.0"
#define MI(i,label) O("\t%d. %s", i, label);
#define NL() O("\n");

Rec b1, b2;

/////////////////////////////////////////////////////////////////////////////////////////
//									PRINTS											   //
/////////////////////////////////////////////////////////////////////////////////////////

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

V get_num(UJ *num, C buf[])
{
	C c = '1', i = 0;
	for (*num = 0; c != '\n'; i++) {
		c = buf[i];
		if (c == '\n') 
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
I rec_delete(I num)
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

Rec rec_get(UJ id) {
	R &b1;
}


V scr_search_1_1(I fld) 
{	
	C string[200];
	UJ num;

	O("please, enter keyword:  ");
	fgets(string, 200, stdin);
	O("\n\tSearch hits:\n\n");
	rec_search(fld, string);
	NL();
}

V scr_displayall_6_1(I fld)
{
	rec_sort(fld);
}

I rec_display_1(UJ id)
{
	if (id > 1)R 0;
	recprint_compressed(&b1);
}

I rec_display_2(UJ id)
{
	if (id > 1)R 0;
	recprint_compressed(&b1);
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


/////////////////////////////////////////////////////////////////////////////////////////
//									SCREENS											   //
/////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////
V scr_editrec_4_2(I fld, Rec origin) 
{
	UJ num;
	C buf[300];

	O("fld %d; fld_year %d; fld_pages %d\n", fld, fld_year,fld_pages);


	if ((fld != fld_year) && (fld != fld_pages)) {
		O("!!\n");
		O("old value:\t%s\n", *(origin + rec_field_offsets[fld]));
		O("new value:\t");
		
		scanf("%s", origin + rec_field_offsets[fld]);
	}
	else {
		O("!\n");
		O("old value:\t%d\n", *(origin + rec_field_offsets[fld]));
		O("new value:\t");
		get_num(&num, buf);
		while (num == -1) {
			O("not a number. enter again\n");
			get_num(&num, buf);
		}

		origin->year = num;
	}
}
//////////////////////////////////////////
I scr_editrec_4_1(UJ id)
{
	Rec origin;
	UJ num;

	if (id == -1) 
 		R 0;
 	else {
 		origin = rec_get(id);

	 	if ((rec_display_2(id))) {
	 		recprint_formated(origin);
	 		while(1) { 
				input(&num, 6, "\tSelect field to edit:  ");

				SW(num) {
					CS(0, R 0);
					CS(1, scr_editrec_4_2(fld_title, origin););
					CS(2, scr_editrec_4_2(fld_author, origin););
					CS(3, scr_editrec_4_2(fld_year, origin););
					CS(4, scr_editrec_4_2(fld_publisher, origin););
					CS(5, scr_editrec_4_2(fld_pages, origin););
					CS(6, scr_editrec_4_2(fld_subject, origin););
				}
		 	}
	 	}
	 	else
	 		O("ERROR: no such record\n");
	}

	R 1;
}

I scr_search_1(UJ *command)
{
	NL()
	O("\tSearch record\n");
	O("\t-------------\n");
	NL();
	MI(1, "By Year\n")
	MI(2, "By Title\n")
	MI(3, "By Author\n")
	MI(4, "By Subject\n")
	NL()
	MI(0, "Main Menu\n")
	NL()
	
	input(command, 4, "select field:  ");

	SW(*command) {
		CS(0, R 0);
		CS(1, scr_search_1_1(fld_year))
		CS(2, scr_search_1_1(fld_title))
		CS(3, scr_search_1_1(fld_author))
		CS(4, scr_search_1_1(fld_subject))
		CD:O("\nERR: unknown command\n\n");
	}

	R get_yn("search by another filed?");
}

I scr_addrec_2(UJ *command)
{
	C buf[15];
	UJ id;
	Rec str = malloc(SZ_REC);
	NL()
	O("\tAdd record\n");
	O("\t-------------\n");
	NL();

	O("TITLE:  ");
	get_line(str->title, 201);
	O("AUTHOR:  ");
	get_line(str->author, 51);

	O("YEAR:  ");
	get_line(buf, 15);
	get_num(&id, buf);
	while (id == -1) {
		O("not a number. enter again\n");
		get_num(&id, buf);
	}
	str->year = id;

	O("PUBLISHER:  ");
	get_line(str->publisher, 101);

	O("PAGES:  ");
	get_line(buf, 15);
	get_num(&id, buf);
	while (id == -1) {
		O("not a number. enter again\n");
		get_num(&id, buf);
	}
	str->pages = id;
	O("SUBJECT:  ");
	get_line(str->subject, 2001);

	str->rec_id = next_id();
	rec_add(str);

	O("[OK: record created with ID %lu]\n", str->rec_id);

	free(str);
	R get_yn("create another one?");
}

I scr_deleterec_3(UJ *num)
{
	C buf[300];
	NL()
	O("\nDelete record\n");
	O("\t-------------\n");
	NL();
 	O("enter book ID or press any letter to go back to main menu\n");
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

I scr_editrec_4(UJ *num)
{
	C buf[15];
	I i = 1;
	NL()
	O("\tEdit record\n");
	O("\t-------------\n");
	NL();
	O("enter book ID or press any letter to go back to main menu  ");
	get_line(buf, 15);
 	get_num(num, buf);

	if (!scr_editrec_4_1(*num)) 
		R 0;		

	
 	R get_yn("edit another one?");
}

I scr_displayrec_5(UJ *num)
{
	C buf[300];
	NL()
	O("\tDisplay record\n");
	O("\t-------------\n");
	NL();	
	O("enter book ID or press any letter to go back to main menu  ");
 	get_line(buf, 15);
 	get_num(num, buf);
 	if (*num == -1) 
 		R 0;

 	if (rec_display_1(*num));
 	else
 		O("ERROR: no such record\n");
 	R get_yn("display another one?");
}

I scr_displayall_6(UJ *num)
{
	NL()
	O("\tDisplay all records\n");
	O("\t-------------\n");
	NL();
	MI(1, "Sorted by ID")
	MI(2, "Sorted by Year")
	MI(3, "By Title")
	MI(4, "By Author")
	NL()
	MI(0, "Main Menu")
	NL()
	input(num, 4, "select command:  ");

	SW(*num) {
		CS(0, R 0);
		CS(1, scr_displayall_6_1(7));			// by id
		CS(2, scr_displayall_6_1(fld_year));
		CS(3, scr_displayall_6_1(fld_title));
		CS(4, scr_displayall_6_1(fld_author));
	}

	R get_yn("sort by another field?");
}

V scr_dbstat_7()
{
	NL()
	O("\tDatabase status\n");
	O("\t-------------\n");
	NL();	
	db_stat();
	O("press any key to return to main menu  ");
	getchar();
}

V scr_dbvacuum_8()
{
	NL()
	O("\tVacuum database\n");
	O("\t-------------\n");
	NL();	
	db_vacuum();
	O("press any key to return to main menu  ");
	getchar();
}


I scr_main_0(UJ *command)
{
	NL();
	O("\tMain menu\n");
	O("\t-----------------------------\n");
	MI(1, "Search records\n")
	MI(2, "Add record\n")
	MI(3, "Delete record\n")
	MI(4, "Edit record\n")
	MI(5, "Display record\n")
	MI(6, "Display all records\n")
	MI(7, "Database status\n")
	MI(8, "Vacuum database\n")
	MI(9, "Import CSV file\n")
	NL()
	MI(0, "Exit program\n")
	NL()

	input(command, 9, "select command: ");

			// 	each case iterates only if iterating function sends a request 
	SW(*command) {
		CS(0, R 0);	 											// 	exit program
		CS(1, while(scr_search_1(command)); 	R 1;);			//	search record
		CS(2, while(scr_addrec_2(command)); 	R 1;);			// 	add record
		CS(3, while(scr_deleterec_3(command)); 	R 1;);			//	delete record
		CS(4, while(scr_editrec_4(command)); 	R 1;);			//	edit record 
		CS(5, while(scr_displayrec_5(command)); R 1;);			//	display record by id
		CS(6, while(scr_displayall_6(command)); R 1;);			//	display all records
		CS(7, scr_dbstat_7());									//	database stat
		CS(8, scr_dbvacuum_8());								//	vacuum database
	/*	CS(9, csv_9());											//	import csv file
*/
	}

	// R get_yn("MAIN MENU: do anything else?");
}


I main()
{
	b1 = malloc(SZ_REC);
	b2 = malloc(SZ_REC);

	UJ command;
	b1->rec_id = 0;
	b1->pages = 423;
	b1->year = 1996;
	strcpy(b1->publisher, "EKSMO");
	strcpy(b1->title, "CHAPAEV AND VOID");
	strcpy(b1->author, "Pelevin V. O.");
	strcpy(b1->subject, "Pyotr Pustota is a poet who has fled from Saint Petersburg to Moscow and who takes up the identity of a Soviet political commissar and meets a strange man named Vasily Chapayev who is some sort of an army commander. He spends his days drinking samogon, taking drugs and talking about the meaning of life with Chapayev.");
	
	b2->rec_id = 1;
	b2->pages = 543;
	b2->year = 1992;
	strcpy(b2->publisher, "EKSMO");
	strcpy(b2->title, "OMON RA");
	strcpy(b2->author, "Pelevin V. O.");
	strcpy(b2->subject, "The protagonist is Omon Krivomazov, who was born in Moscow post-World War II. The plot traces his life from early childhood. In his teenage years, the realization strikes him that he must break free of Earth's gravity to free himself of the demands of the Soviet society and the rigid ideological confines of the state.");

	NL();
	banner();
	while (scr_main_0(&command));

	free(b1);
	free(b2);
	R 0;
}



