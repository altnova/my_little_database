#include <stdio.h>
#include "___.h"

I input_number(I *num, S prompt, S errmsg){
    C line[4096];
    while(O("%s: ", prompt) > 0 && fgets(line, SZ(line), stdin) != 0)
        if(sscanf(line, "%d", num) == 1)R 0;else O("%s\n", errmsg);
    R EOF;
}

I main(V){
	I n;
    input_number(&n, "say 42", "try again");
	O("%d\n", n);
} 

//:~