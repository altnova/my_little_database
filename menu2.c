#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define R return
#define O printf
#define I int
#define C char
#define UJ unsigned long
#define S char*
#define CS(n,x)	case n:x;break;
#define VER "1.0.0"
#define MI(i,label) O("\t%d. %s\n", i, label);
#define NL() O("\n");
#define SW switch
#define VER "1.0.0"

S fld_year = "fld_year";
S fld_title = "fld_title";
S fld_author = "fld_author";
S fld_subject = "fld_subject";

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

void input(UJ *command, I num)
{
	O("select item:_ ");
	get_num(command);

	while(*command == -1 || *command > num) {
		O("\x1B[31mERROR:\x1B[0m unknown command\n");
		O("select item:_ ");
		get_num(command);
	}
}


/////////////////////////////////////////////////////////////////////////////////////////
/*	display record 	*/
void scr_displayrec_5() {}
/*	display all records */
void scr_displayall_6() {}
/* 	display database status */
void scr_dbstat_7() {}
/* 	vacuum database 	*/
void scr_dbvacuum_8() {}
void csv_9() {}
/////////////////////////////////////////////////////////////////////////////////////////
I rec_delete(I num)
{ 
	R num % 2;
}

I rec_add()
{ O("rec_add\n"); R 1;}

void scr_search_1_1(S fld) 
{
	printf("%s\n", fld);
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
	
	input(command, 4);

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
	UJ id = rec_add();
	if (id == -1) {
		O("ERROR\n");
		R 1;
	}

	O("[OK: record created with ID %d]\n", id);
	R menu("create another one? [y/n]  ") == 'y' ? 1 : 0;
}

I scr_deleterec_3(UJ *command)
{
 	O("enter book ID or press any letter to go back to main menu\n");
 	get_num(command);

 	if (*command == -1) 
 		R 0;
 
 	if (rec_display(*command)) {
 		if (menu("this one? [y/n]  ") == 'y') {
 			rec_delete(*command);
 			O("record ID %lu deleted\n", *command);
 		}
 	}
 	else 
 		O("ERROR: no such record\n");

	R menu("delete another one? [y/n]  ") == 'y' ? 1 : 0;
}

I scr_editrec_4(UJ *command)
{
	O("enter book ID or press any letter to go back to main menu\n");
 	get_num(command);

 	if (*command == -1) 
 		R 0;
 
 	if (rec_display(*command)) {
 		if (menu("this one? [y/n]  ") == 'y') {
 			rec_edit(*command);
 			O("[OK: Record ID %lu updated]\n", *command);
 		}
 	}
 	else
 		O("ERROR: no such record\n");

 	R menu("delete another one? [y/n]  ") == 'y' ? 1 : 0;
}


I scr_main_0(UJ *command)
{
	NL();
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

	input(command, 9);

// 	each case iterates only if iterating function sends a request 
	SW(*command) {

		CS(0, R 0);	 								// 	exit program
		CS(1, while(scr_search_1(command)););		//	search record
		CS(2, while(scr_addrec_2(command)););		// 	add record
		CS(3, while(scr_deleterec_3(command)););		//	delete record
		CS(4, while(scr_editrec_4(command)););		//	edit record 
	/*	CS(5, while(scr_displayrec_5(command)););	//	display record by id
		CS(6, while(scr_displayall_6(command)););	//	display all records
		CS(7, scr_dbstat_7());						//	database stat
		CS(8, scr_dbvacuum_8());							//	vacuum database
		CS(9, csv_9());								//	import csv file
*/
	}

	R menu("MAIN MENU: do anything else? [y/n]  ") == 'y' ? 1 : 0;
}


I main()
{
	UJ command;
	NL();
	banner();
	while (scr_main_0(&command));
	R 0;
}



