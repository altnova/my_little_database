#include <stdio.h>
#include "___.h"

I input_number(I *num, S prompt, S errmsg){
    char line[4096];

    while(O("%s: ", prompt) > 0 && fgets(line, SZ(line), stdin) != 0)
    {
        if(sscanf(line, "%d", num) == 1)
            R 0;
        O("%s\n", errmsg);
    }
    R EOF;
}

I main(V){
	I n;
	input_number(&n);
	O("%d\n", n);
} 

//:~