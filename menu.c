#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "books.h"

#define VER "1.0.0"
#define MI(i,label) O("\t%d. %s\n", i, label);
#define NL() O("\n");

V scr_main_0();
V scr_search_1();
V scr_search_1_1(I);

void banner() {
	O("\tAmazon Kindle Database v%s\n", VER);
	O("\t_____________________________\n");
}

I input_num(int *number, S prompt) {
    C line[4096];
    while(O("%s: ", prompt) > 0 && fgets(line, sizeof(line), stdin) != 0) {
        if (sscanf(line, "%d", number) == 1)
            return 0;
        printf("ERR: invalid format\n");
    }
    return EOF;
}

V scr_search_1() {
	I choice;

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
	input_num(&choice, "Select field");
	SW(choice){
		CS(0, R)
		CS(1, scr_search_1_1(fld_year))
		CS(2, scr_search_1_1(fld_title))
		CS(3, scr_search_1_1(fld_author))
		CS(4, scr_search_1_1(fld_subject))
		CD:O("\nERR: unknown command\n\n");
	}
}

V scr_main_0() {
	I choice;

	NL()
	banner();
	NL();
	MI(1, "Search record")
	MI(2, "Add record")
	MI(3, "Delete record")
	MI(4, "Edit record")
	MI(5, "Display record")
	MI(6, "Display all records")
	MI(7, "Database status")
	NL()
	MI(0, "Exit program")
	NL()

	input_num(&choice, "Select menu item");
	SW(choice){
		CS(0, O("Goodbye.\n"); exit(0))
		CS(1, scr_search_1())
		CD:O("\nERR: unknown command\n\n");
	}
}

V scr_search_1_1(I fld) {
}

I main() {
	while(1)scr_main_0();
	R 0;
}



