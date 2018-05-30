#CC=gcc-8
CC=gcc
#CCOPTS=-mavx2 -ffast-math -march=native -flto -mfpmath=sse -funroll-loops -Ofast
CCOPTS=-Og -g
#VLG=/opt/valgrind/bin/valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes
VLG=
OUT=../../bin

all: tests

tests: bag vec tri set hsh

bag:
	$(CC) -DRUN_TESTS_BAG $(CCOPTS) -o $(OUT)/bag ../utl/trc.c bag.c
	$(VLG) $(OUT)/bag

vec:
	$(CC) -DRUN_TESTS_VEC $(CCOPTS) -o $(OUT)/vec ../utl/trc.c vec.c 
	$(VLG) $(OUT)/vec

tri:
	$(CC) -DRUN_TESTS_TRI $(CCOPTS) -o $(OUT)/tri ../utl/trc.c tri.c
	$(VLG) $(OUT)/tri

set:
	$(CC) -DRUN_TESTS_SET $(CCOPTS) -o $(OUT)/set ../utl/trc.c ../alg/bin.c vec.c set.c
	$(VLG) $(OUT)/set

hsh:
	$(CC) -DRUN_TESTS_HSH $(CCOPTS) -o $(OUT)/hsh ../utl/clk.c ../utl/rnd.c ../utl/trc.c vec.c hsh.c
	$(VLG) $(OUT)/hsh

#//:~