#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define R return
#define O printf
#define I int
#define C char
#define UJ unsigned long

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
	// printf("%d!\n", num);
}

/*	main menu	*/
I scr_main_0(UJ *num); 
/*	exit program	*/
// void scr_exit_0() {}
/*	search record 	*/
I scr_search_1(UJ *num);
void scr_search_1_1();
/*	add record 	*/
// void scr_addrec_2() {}
/*	delete record 	*/
// void scr_deleterec_3() {}
/*	edit record 	*/
// void scr_editrec_4() {}
/*	display record 	*/
// void scr_displayrec_5() {}
/*	display all records */
// void scr_displayall_6() {}
/* 	display database status */
// void scr_dbstat_7() {}
/* 	vacuum database 	*/
// void scr_dbvacuum_8() {}

void input(UJ *command)
{
	get_num(command);

	while(*command == -1) {
		O("\nERROR: unknown command\n\n");
		O("select menu item: ");
		get_num(command);
	}

}

I scr_search_1(UJ *command)
{
	O("1. by year\n2. by title\n3. by author\n");
	O("4. by subject\n\n0. main menu\n\n");
	input(command);

	switch (*command) {
		case 0:
		R 0;
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
		R 1;
		break;
	}
	R 0;
}



I scr_main_0(UJ *command)
{
	O("1. search record\n2. add record\n3. delete record\n");
	O("4. edit record\n5. display record\n6. display all records\n");
	O("7. database status\n8. vaccum database\n\n0. exit program\n\n");
	O("select menu item: ");
	get_num(command);

	switch (*command) {
		case 0:
		R 0;	 			// 	exit program
		break;

		case 1:
		for  (;scr_search_1(command); printf("-----------------------------\n"));
		R 1;
	
		break;

	/*	case 2:
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
*/
		default:
		O("\nERROR: unknown command\n\n");
		R 1;
	}

R 0;
}


I main()
{
	UJ command;
	while (scr_main_0(&command));
	R 0;
}



