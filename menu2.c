#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "books.h"
#define VER "1.0.0"
#define MI(i,label) O("\t%d. %s\n", i, label);
#define NL() O("\n");

typedef struct books {
	UJ book_id;
	H pages;
	H year;
	C publisher[101];
	C title[201];
	C author[51];
	C subject[2001];
} Book;





void banner() {
	O("\tAmazon Kindle Database v%s\n", VER);
	O("\t_____________________________\n");
}

/*	asks user to tap something */
C menu(const C *hint) 
{
    C c;
    O("%s", hint);
    c = getchar();
    getchar();
    return c;
}

/*	returns number from input or error (-1)	*/
void get_num(UJ *num)
{
	C c = '1';
	for (*num = 0; c != '\n'; ) {
		scanf("%c", &c);
		if (c == '\n') 
			break;

		if (c < '0' || c > '9') {
			while (c != '\n')
				scanf("%c", &c);
			*num = -1;
			R;
		}
		*num *= 10;
		*num += c - '0';
	}
}

void input(UJ *command, I num, const S request)
{
	O(request);
	get_num(command);

	while(*command == -1 || *command > num) {
		O("\x1B[31mERROR:\x1B[0m unknown command\n");
		O(request);
		get_num(command);
	}
}


/////////////////////////////////////////////////////////////////////////////////////////
void csv_9() {}
/////////////////////////////////////////////////////////////////////////////////////////
I rec_delete(I num)
{ 
	R num % 2;
}

I rec_add()
{ O("rec_add\n"); R 1;}

void scr_search_1_1(I fld) 
{	
	C string[200];
	UJ num;

	O("please, enter keyword:  ");
	fgets(string, 200, stdin);
	O("\tSearch results:\n\n");
	rec_search(fld, string);
	NL();
}

void scr_displayall_6_1(I fld)
{
	rec_sort(fld);
}

I rec_display(UJ id)
{
	O("rec_display ID %lu\n", id);
	R id%2;
}

void rec_edit(UJ id) {
	O("rec_edit ID %lu\n", id);
	// R id%2;
}

V 	db_stat()
{
	O("\nData file:\t~/arina/books.dat\t(800 bytes)\nIndex file:\t~/arina/books.idx\t(64 bytes)\n");
	O("Records\t\t2\nDeleted\t\t0\nLast ID:\t1\n\n");
}

V 	db_vacuum()
{
	O("[OK: Successfuly purged 2 deleted records.]\n\n");
}
/////////////////////////////////////////////////////////////////////////////////////////
//									SCREENS											   //
/////////////////////////////////////////////////////////////////////////////////////////

I scr_search_1(UJ *command)
{
	NL()
	O("\tSearch record\n");
	O("\t-------------\n");
	NL();
	MI(1, "By Year")
	MI(2, "By Title")
	MI(3, "By Author")
	MI(4, "By Subject")
	NL()
	MI(0, "Main Menu")
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

	R menu("search by another field? [y/n] ") == 'y' ? 1 : 0;
}

I scr_addrec_2(UJ *command)
{
	UJ id;
	NL()
	O("\tAdd record\n");
	O("\t-------------\n");
	NL();

	id = rec_add();
	if (id == -1) {
		O("ERROR\n");
		R 1;
	}

	O("[OK: record created with ID %d]\n", id);
	R menu("create another one? [y/n]  ") == 'y' ? 1 : 0;
}

I scr_deleterec_3(UJ *num)
{
	NL()
	O("\nDelete record\n");
	O("\t-------------\n");
	NL();
 	O("enter book ID or press any letter to go back to main menu\n");
 	get_num(num);

 	if (*num == -1) 
 		R 0;
 
 	if (rec_display(*num)) {
 		if (menu("this one? [y/n]  ") == 'y') {
 			rec_delete(*num);
 			O("record ID %lu deleted\n", *num);
 		}
 	}
 	else 
 		O("ERROR: no such record\n");

	R menu("delete another one? [y/n]  ") == 'y' ? 1 : 0;
}

I scr_editrec_4(UJ *num)
{
	NL()
	O("\tEdit record\n");
	O("\t-------------\n");
	NL();
	O("enter book ID or press any letter to go back to main menu\n");
 	get_num(num);

 	if (*num == -1) 
 		R 0;
 
 	if (rec_display(*num)) {
 		if (menu("this one? [y/n]  ") == 'y') {
 			rec_edit(*num);
 			O("[OK: Record ID %lu updated]\n", *num);
 		}
 	}
 	else
 		O("ERROR: no such record\n");

 	R menu("edit another one? [y/n]  ") == 'y' ? 1 : 0;
}

I scr_displayrec_5(UJ *num)
{
	NL()
	O("\tDisplay record\n");
	O("\t-------------\n");
	NL();	
	O("enter book ID or press any letter to go back to main menu\n");
 	get_num(num);
 	if (*num == -1) 
 		R 0;

 	if (rec_display(*num));
 	else
 		O("ERROR: no such record\n");
 	R menu("display another one? [y/n]  ") == 'y' ? 1 : 0;
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

	R menu("sort by another field? [y/n]  ") == 'y' ? 1 : 0;
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
	O("\t-------------\n");
	MI(1, "Search record")
	MI(2, "Add record")
	MI(3, "Delete record")
	MI(4, "Edit record")
	MI(5, "Display record")
	MI(6, "Display all records")
	MI(7, "Database status")
	MI(8, "Vacuum database")
	MI(9, "Import CSV file")
	NL()
	MI(0, "Exit program")
	NL()

	input(command, 9, "select command:  ");

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

	// R menu("MAIN MENU: do anything else? [y/n]  ") == 'y' ? 1 : 0;
}


I main()
{
	UJ command;
	NL();
	banner();
	while (scr_main_0(&command));
	R 0;
}



