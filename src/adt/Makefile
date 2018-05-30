#CC=gcc-8
CC=gcc
#CCOPTS=-mavx2 -ffast-math -march=native -flto -mfpmath=sse -funroll-loops -Ofast
CCOPTS=-Og -g
#VLG=/opt/valgrind/bin/valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes
VLG=
OUT=../../bin
COMMON=../utl/trc.c bag.c

all: tests

tests: bag vec tri set hsh

bag:
	$(CC) -DRUN_TESTS_BAG $(CCOPTS) -o $(OUT)/bag ../utl/trc.c bag.c
	$(VLG) $(OUT)/bag

vec:
	$(CC) -DRUN_TESTS_VEC $(CCOPTS) -o $(OUT)/vec $(COMMON) vec.c 
	$(VLG) $(OUT)/vec

tri:
	$(CC) -DRUN_TESTS_TRI $(CCOPTS) -o $(OUT)/tri $(COMMON) tri.c
	$(VLG) $(OUT)/tri

set:
	$(CC) -DRUN_TESTS_SET $(CCOPTS) -o $(OUT)/set $(COMMON) ../alg/bin.c vec.c set.c
	$(VLG) $(OUT)/set

hsh:
	$(CC) -DRUN_TESTS_HSH $(CCOPTS) -o $(OUT)/hsh $(COMMON) ../utl/clk.c ../utl/rnd.c vec.c hsh.c
	$(VLG) $(OUT)/hsh

#//:~