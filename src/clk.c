#include <time.h>
#include "___.h"
#include "trc.h"

clock_t start,end;

V clk_start() {
	start = clock();	
}

UJ clk_stop() {
	end = clock();
	UJ r = (end - start) * 1E3 / CLOCKS_PER_SEC;
	start = end; //< allow chained clk_stop()
	R r;

}

#ifdef RUN_TESTS
I main(){
	LOG("clk_test");
	clk_start();
	UJ tmp = 0;
	DO(1E7,tmp += 1)
	T(TEST, "%lu additions\t=> %lums", tmp, clk_stop());
	tmp = 0;
	DO(1E8,tmp += 1)
	T(TEST, "%lu additions\t=> %lums", tmp, clk_stop());
	R0;
}
#endif


//:~
