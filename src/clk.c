#include <time.h>

clock_t start,end;

V clk_start() {
	start = clock();	
}

UJ clk_stop() {
	end = clock();
	UJ r = (end - start) * 1E3 / CLOCKS_PER_SEC;
	start = end; //< allow chained clk_stop()
}

//:~
