#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "ryba.c"



/////////////////////////////////////////////////////////////////////////////////////////
//									SCREENS											   //
/////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////
V scr_search_1_1(I fld) 
{	
	C string[200];
	UJ num;

	O("please, enter keyword:  ");
	get_line(string, 200);
	O("\n\tSearch hits:\n\n");
	rec_search(fld, string);
	NL();
}

V scr_displayall_6_1(I fld)
{
	rec_sort(fld);
}


//	gets editing filed and a pointer to origin struct
V scr_editrec_4_2(I fld, Book *origin) 
{
	UJ num;
	C buf[csv_max_field_widths[fld]];

	O("fld %d; fld_year %d; fld_pages %d\n", fld, fld_year,fld_pages);

	O("old value:  ");

	if (fld == fld_year || fld == fld_pages) 
		O("%d\n", *(origin + rec_field_offsets[fld])); 				//<<<<<<<<<<<<<
	else
		O("%s\n", *(origin + rec_field_offsets[fld]));				//<<<<<<<<<<<<<

	O("new value:  ");
	get_line(buf, csv_max_field_widths[fld]);

	if (fld == fld_year || fld == fld_pages) {
		get_num(&num, buf);
		while (num == -1) {
			O("not a number. enter again  ");
			get_line(buf, csv_max_field_widths[fld]);
			get_num(&num, buf);
		}
		*(origin + rec_field_offsets[fld]) = num;					//<<<<<<<<<<<<<
	}
	else 
		strcpy((S)(origin + rec_field_offsets[fld] ), buf);			//<<<<<<<<<<<<<

}
//////////////////////////////////////////

//	returns 0 if id does not exist
I scr_editrec_4_1(UJ id)
{
	Book *origin;
	UJ num;

	if (id == -1) 
 		R 0;

	if ((rec_display_fotmated(id))) {
		origin = rec_get(id);
		while(1) { 
			input(&num, 6, "\tSelect field to edit:  ");
			if (num == 0) 
					R 0;
			num = 	num == 1 	? fld_title 	:
					num == 2 	? fld_author 	:
					num == 3	? fld_year		:
					num == 4 	? fld_publisher	:
					num == 5	? fld_pages		:
								fld_subject;
		scr_editrec_4_2(num, origin);

	 	}
	}
	else
		O("ERROR: no such record\n");
	
	R 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
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
	Book str;
	NL()
	O("\tAdd record\n");
	O("\t-------------\n");
	NL();

	O("TITLE:  ");
	get_line(str.title, 201);
	O("AUTHOR:  ");
	get_line(str.author, 51);

	O("YEAR:  ");
	get_line(buf, 15);
	get_num(&id, buf);
	while (id == -1) {
		O("not a number. enter again\n");
		get_num(&id, buf);
	}
	str.year = id;

	O("PUBLISHER:  ");
	get_line(str.publisher, 101);


	O("PAGES:  ");
	get_line(buf, 15);
	get_num(&id, buf);
	while (id == -1) {
		O("not a number. enter again\n");
		get_num(&id, buf);
	}
	str.pages = id;
	O("SUBJECT:  ");
	get_line(str.subject, 2001);

	str.book_id = next_id();
	rec_add(str);

	O("[OK: record created with ID %lu]\n", str.book_id);
	R get_yn("create another one?");
}

I scr_deleterec_3(UJ *num)
{
	C buf[15];
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
	C buf[20];
	NL()
	O("\tDisplay record\n");
	O("\t-------------\n");
	NL();	
	O("enter book ID or press any letter to go back to main menu  ");
 	get_line(buf, 15);
 	get_num(num, buf);
 	if (*num == -1) 
 		R 0;

 	if 		(rec_display_1(*num));
 	else	O("ERROR: no such record\n");
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
	O("\t-------------\n");
	MI(1, "Search record\n")
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

	// R get_yn("MAIN MENU: do anything else?");
}


I main()
{
	UJ command;
	b1.book_id = 0;
	b1.pages = 423;
	b1.year = 1996;
	strcpy(b1.publisher, "EKSMO");
	strcpy(b1.title, "CHAPAEV AND VOID");
	strcpy(b1.author, "Pelevin V. O.");
	strcpy(b1.subject, "Pyotr Pustota is a poet who has fled from Saint Petersburg to Moscow and who takes up the identity of a Soviet political commissar and meets a strange man named Vasily Chapayev who is some sort of an army commander. He spends his days drinking samogon, taking drugs and talking about the meaning of life with Chapayev.");
	
	b2.book_id = 1;
	b2.pages = 543;
	b2.year = 1992;
	strcpy(b2.publisher, "EKSMO");
	strcpy(b2.title, "OMON RA");
	strcpy(b2.author, "Pelevin V. O.");
	strcpy(b2.subject, "The protagonist is Omon Krivomazov, who was born in Moscow post-World War II. The plot traces his life from early childhood. In his teenage years, the realization strikes him that he must break free of Earth's gravity to free himself of the demands of the Soviet society and the rigid ideological confines of the state.");


	NL();
	banner();
	while (scr_main_0(&command));
	R 0;
}



