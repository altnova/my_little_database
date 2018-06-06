//! \file clk.c \brief precision timer

#include "../___.h"
#include "clk.h"

Z clock_t start,end;

clock_t clk_start() {
	R start = clock();	
}

UJ clk_diff(clock_t s, clock_t e) {
	R(e - s) * 1E3 / CLOCKS_PER_SEC;
}

UJ clk_stop() {
	end = clock();
	UJ r = clk_diff(start,end);
	start = end; //< allow chained clk_stop()
	R r;
}

Z UJ clk_test(UJ n) {
	LOG("clk_test");
	UJ tmp = 0;
	DO(n,tmp+=1)
	T(TEST, "%lu rounds\t=> %lums", tmp, clk_stop());
	R tmp;
}

#ifdef RUN_TESTS_CLK
I main(){
	LOG("clk_main");
	UJ epoch = clk_start(); //< init clock

	clk_test(1E7/2);
	clk_test(1E7);
	clk_test(1E8/2);
	clk_test(1E8);

	T(TEST, "total exec time\t=> %lums", clk_diff(epoch, clk_start()));

	R0;
}
#endif


//:~
