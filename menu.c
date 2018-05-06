#include <stdio.h>
#include <stdlib.h>
#include <math.h>

I get_num()
{
	I num = 0, i;
	C c = '1';
	for (i = 0; c != '\n'; i++) {
		c = getchar();
		if (c == '\n')
			break;
		else 
			c -= '0';

		if (c < 0 || c > 9)	 		// if c is not a digit
			R -1;
		num += c * pow(10, i);
	}
	R num;
}

/*	main menu	*/
void scr_main_0();

/*	exit program	*/
void scr_exit_0();

/*	search record 	*/
void scr_search_1();

/*	add record 	*/
void scr_addrec_2();

/*	delete record 	*/
void scr_deleterec_3();

/*	edit record 	*/
void scr_editrec_4();

/*	display record 	*/
void scr_displayrec_5();

/*	display all records */
void scr_displayall_6();

/* 	display database status */
void scr_dbstat_7();

/* 	vacuum database 	*/
void scr_dbvacuum_8();



void scr_main_0()
{
	// I command;
	O("1. search record\n2. add record\n3. delete record\n");
	O("4.edit record\n5. display record\n6. display all records\n");
	O("7. database status\n8. vaccum database\n\n 0. exit program\n");
	// command = get_num();

	switch (get_num()) {
		case 0:
		scr_exit_0(); 		// 	exit program
		break;

		case 1:
		scr_search_1():		//	search records
		break;

		case 2:
		scr_addrec_2();		// 	add record
		break;

		case 3:
		scr_deleterec_3();	//	delete record
		break;

		case 4:
		scr_editrec_4();	//	edit record
		break;

		case 5:
		scr_displayrec_5();	//	display record
		break;

		case 6:
		scr_displayall_6();	//	display all records
		break;

		case 7:
		scr_dbstat_7();		//	database status
		break;

		case 8:
		scr_dbvacuum_8();	//	vacuum database 
		break;

		default:
		O("input error\n\n");
		scr_main_0();
	}



}



