//!\file usr.c \brief user input utilities

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../___.h"
#include "usr.h"

#define STR_SET "%[a-zA-Z0-9\"'-,.$%*/+-_=^~#?|!():;<> \\]\n\r"

ext I usr_input_num(I *num, S prompt, S errmsg){
    C line[LINE_BUF];
    O("\n\n");
    W(O("%s: ", prompt)>0&&fgets(line, SZ(line), stdin)!=0)
        if(sscanf(line, "%d", num)==1)R0;else O("%s\n", errmsg);
    R EOF;
}

I usr_input_str(S str, S prompt, S errmsg){
    C line[LINE_BUF];
    W(O("%s", prompt)>0&&fgets(line, SZ(line), stdin)!=0)
        if(sscanf(line, STR_SET, str)==1)R0;else {
            sz len = scnt(line); C c = *line;
            P(len==1&&c==10, 0); //< LF - empty line
            O("%d (%lu): %s\n", *line, scnt(line), errmsg);
        }
    R EOF;
}

#ifdef RUN_TESTS_USR
I main(V){
	//USR_LOOP(usr_input_str(str, "Search query", "Invalid input"),
    //    O("You have entered: %s\n", str);
	//)
} 
#endif

//:~