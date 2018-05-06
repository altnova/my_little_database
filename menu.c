#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define R return
#define O printf
#define I int
#define C char
#define UJ unsigned long

/*	returns number from input or error (-1)	*/
UJ get_num()
{
	UJ num = 0;
	C c = '1';

	while (c != '\n') {
		scanf("%c", &c);
		if (c == '\n') 
			break;

		if (c < '0' || c > '9') {
			while (c != '\n')
				scanf("%c", &c);
			R -1;
		}
		num *= 10;
		num += c - '0';
	}

	R num;
}

/*	main menu	*/
void scr_main_0(); 

/*	exit program	*/
void scr_exit_0() {}

/*	search record 	*/
void scr_search_1();
void scr_search_1_1();

/*	add record 	*/
void scr_addrec_2() {}

/*	delete record 	*/
void scr_deleterec_3() {}

/*	edit record 	*/
void scr_editrec_4() {}

/*	display record 	*/
void scr_displayrec_5() {}

/*	display all records */
void scr_displayall_6() {}

/* 	display database status */
void scr_dbstat_7() {}

/* 	vacuum database 	*/
void scr_dbvacuum_8() {}

void scr_search_1()
{
	I command;
	O("1. by year\n2. by title\n3. by author\n");
	O("4. by subject\n\n0. main menu\n");
	O("select menu item: ");

	command = get_num();
	switch (command) {
		case 0:
		scr_main_0();
		break;

	/*	case 1:
		scr_search_1_1(fld_year);
		break;

		case 2:
		scr_search_1_1(fld_title);
		break;

		case 3:
		scr_search_1_1(fld_author);
		break;

		case 4:
		scr_search_1_1(fld_subject);
		break;*/

		default:
		O("\nERROR: unknown command\n\n");
		scr_search_1();
	}
}



void scr_main_0()
{
	I command;
	O("1. search record\n2. add record\n3. delete record\n");
	O("4. edit record\n5. display record\n6. display all records\n");
	O("7. database status\n8. vaccum database\n\n0. exit program\n");
	O("select menu item: ");
	command = get_num();

	switch (command) {
		case 0:
		scr_exit_0(); 			// 	exit program
		break;

		case 1:
		scr_search_1();			//	search records
		break;

		case 2:
		scr_addrec_2();			// 	add record
		break;

		case 3:
		scr_deleterec_3();		//	delete record
		break;

		case 4:
		scr_editrec_4();		//	edit record
		break;

		case 5:
		scr_displayrec_5();		//	display record
		break;

		case 6:
		scr_displayall_6();		//	display all records
		break;

		case 7:
		scr_dbstat_7();			//	database status
		break;

		case 8:
		scr_dbvacuum_8();		//	vacuum database 
		break;

		default:
		O("\nERROR: unknown command\n\n");
		scr_main_0();
	}

}


I main()
{
	scr_main_0();
	R 0;
}



