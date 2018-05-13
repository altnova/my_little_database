#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
// #include "src/cfg.h"
// #include "src/___.h"
#include "ryba.c"



/////////////////////////////////////////////////////////////////////////////////////////
//									SCREENS											   //
/////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////
V scr_search_1_1(I fld) 
{	
	C string[30];
	UJ *ptr, num = 0;
	ptr = &num;

	O("please, enter keyword:  ");

	get_line(string, 30);										//<		asks for a keyword

	if (fld == fld_year || fld == fld_pages) {  				//<	 	if it should be a number, requires for a number

		get_num(ptr, string);
		printf("%d\n", *ptr);

		while (*ptr == -1) {
			O("not a number. enter again  ");
			get_line(string, 30);
			get_num(ptr, string);
		}
	}

	O("\n\tSearch hits:\n\n");
	rec_search(fld, string);

	NL();
	
}


//	gets editing filed and a pointer to origin struct
V scr_editrec_4_2(I fld, Rec origin) 
{
	UJ num = 0, *ptr = &num;

	C buf[csv_max_field_widths[fld] + 1];
	O("\told value:  ");

	if (fld == fld_year || fld == fld_pages) 					//<		field branching because of different printing formats
		O("%hi\n", *(H*)((S)origin + rec_field_offsets[fld]));
	
	else {
		strcpy(buf, (S)((S)origin + rec_field_offsets[fld]));
		O("%s\n", buf);										
	}

	O("\tnew value:  ");
	get_line(buf, csv_max_field_widths[fld]);					//<		asks for a word


	if (fld == fld_year || fld == fld_pages) {					//<	 	if it should be a number, requires for a number
		get_num(ptr, buf);

		while (*ptr == -1) {
			O("not a number. enter again  ");
			get_line(buf, csv_max_field_widths[fld]);
			get_num(ptr, buf);
		}
		*((S)origin + rec_field_offsets[fld]) = *(H*)ptr;						
	}
	else 
		strncpy((S)((S)origin + rec_field_offsets[fld]), buf, csv_max_field_widths[fld] );	
	
}
//////////////////////////////////////////

//	returns 0 if id does not exist
I scr_editrec_4_1(UJ id)
{
	Rec origin;
	UJ num = 0;

	if (id == -1) 
 		R 0;

	if ((rec_display_fotmated(id))) {									// 	if this record exists
		origin = rec_get(id);
		while(1) { 
			num = 0;
			input(&num, 6, "\tSelect field to edit or press 0 to exit:  ");
			
			if (!num) 												// 	iteration stops only if there was an exit request
					R 0;		

			num = 	num == 1 	? 	fld_title 		:
					num == 2 	? 	fld_author 		:
					num == 3	? 	fld_year		:
					num == 4 	? 	fld_publisher	:
					num == 5	? 	fld_pages		:
									fld_subject		;
	
			scr_editrec_4_2(num, origin);
	 	}
	}
	else
		O("ERROR: no such record\n");
	
	R 1;
}

//////////////////////////////////////////////////
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

	if (!*command)
		R 0;

	*command = 	*command == 1 ? 	fld_year 		:
				*command == 2 ? 	fld_title		:
				*command == 3 ? 	fld_author		:
									fld_subject;


	scr_search_1_1(*command);

	R get_yn("search by another filed?  ");
}

I scr_addrec_2()
{
	Rec origin;
	C buf[2001];
	H len;
	NL()
	O("\tAdd record\n");
	O("\t-------------\n");
	NL();

	////////			COLLAPSE		////////////

	// buf = malloc(SZ(C) * 200);
	origin = rec_get(1);

	O("TITLE:  ");
	len = csv_max_field_widths[fld_title];
	get_line(buf, len);

	strncpy((S)((S)origin + rec_field_offsets[fld_title]), buf, csv_max_field_widths[fld_title]);	
	

	O("AUTHOR:  ");
	len = csv_max_field_widths[fld_author];
	get_line(buf, len);
	strncpy((S)((S)origin + rec_field_offsets[fld_author]), buf, csv_max_field_widths[fld_author]);	

	input_number((UJ*)((S)origin + rec_field_offsets[fld_year]), "YEAR:  ", "not a number. enter again:  ");
	// str.year = id;

	O("PUBLISHER:  ");
	len = csv_max_field_widths[fld_publisher];
	get_line(buf, len);
	strncpy((S)((S)origin + rec_field_offsets[fld_publisher]), buf, csv_max_field_widths[fld_publisher]);	

	input_number((UJ*)((S)origin + rec_field_offsets[fld_pages]), "PAGES:  ", "not a number. enter again:  ");

	O("SUBJECT:  ");
	len = csv_max_field_widths[fld_subject];
	get_line(buf, len);

	strncpy((S)((S)origin + rec_field_offsets[fld_subject]), buf, csv_max_field_widths[fld_subject]);	

	origin->rec_id = next_id();

	////////		END OF COLLAPSE		////////////

	rec_add(origin);

	O("[OK: record created with ID %lu]\n", origin->rec_id);
	R get_yn("create another one?");
}

I scr_editrec_4(UJ *id)
{
	C buf[15];

	NL()
	O("\tEdit record\n");
	O("\t-------------\n");
	NL();
	O("enter book ID or press any letter to go back to main menu  ");

	get_line(buf, 15);
 	get_num(id, buf);

	if (!scr_editrec_4_1(*id)) 
		R 0;		

 	R get_yn("edit another one?");
}


I scr_main_0(UJ *command)
{
	I i = 1;
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

		CS(0, R 0;);	 												// 	exit program
		CS(1, while(scr_search_1(command)); 		break;);			//	search record
		CS(2, while(scr_addrec_2()); 				break;);			// 	add record
		CS(3, while(scr_deleterec_3(command)); 		break;);			//	delete record
		CS(4, while(scr_editrec_4(command)); 		break;);			//	edit record 
		CS(5, while(scr_displayrec_5(command)); 	break;);			//	display record by id
		CS(6, while(scr_displayall_6(command)); 	break;);			//	display all records
		CS(7, scr_dbstat_7(); 						break;);			//	database stat
		CS(8, scr_dbvacuum_8(); 					break;);			//	vacuum database
		CS(9, scr_csv_9());												//	import csv file

	}
	// R 1;
	R get_yn("MAIN MENU: do anything else?");							//<		whitout this thing it's going into infinite loop
																		//		don't know why					
}


I main()
{
	UJ command;
	b1.rec_id = 0;
	b1.pages = 423;
	b1.year = 1996;
	strcpy((S)b1.publisher, "EKSMO");
	strcpy((S)b1.title, "CHAPAEV AND VOID");
	strcpy((S)b1.author, "Pelevin V. O.");
	strcpy((S)b1.subject, "Pyotr Pustota is a poet who has fled from Saint Petersburg to Moscow and who takes up the identity of a Soviet political commissar and meets a strange man named Vasily Chapayev who is some sort of an army commander. He spends his days drinking samogon, taking drugs and talking about the meaning of life with Chapayev.");
	
	b2.rec_id = 1;
	b2.pages = 543;
	b2.year = 1992;
	strcpy((S)b2.publisher, "EKSMO");
	strcpy((S)b2.title, "OMON RA");
	strcpy((S)b2.author, "Pelevin V. O.");
	strcpy((S)b2.subject, "The protagonist is Omon Krivomazov, who was born in Moscow post-World War II. The plot traces his life from early childhood. In his teenage years, the realization strikes him that he must break free of Earth's gravity to free himself of the demands of the Soviet society and the rigid ideological confines of the state.");


	NL();
	banner();
	while (scr_main_0(&command));

	R 0;
}


