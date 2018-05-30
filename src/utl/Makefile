#CC=gcc-8
CC=gcc
#CCOPTS=-mavx2 -ffast-math -march=native -flto -mfpmath=sse -funroll-loops -Ofast
CCOPTS=-Og -g
#VLG=/opt/valgrind/bin/valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes
VLG=
OUT=../../bin

all: tests

tests: fio clk rnd usr str

fio:
	$(CC) -DRUN_TESTS_FIO $(CCOPTS) -o $(OUT)/fio trc.c fio.c 
	$(VLG) $(OUT)/fio

clk:
	$(CC) -DRUN_TESTS_CLK $(CCOPTS) -o $(OUT)/clk trc.c clk.c 
	$(VLG) $(OUT)/clk

rnd:
	$(CC) -DRUN_TESTS_RND $(CCOPTS) -o $(OUT)/rnd trc.c rnd.c 
	$(VLG) $(OUT)/rnd

usr:
	$(CC) -DRUN_TESTS_USR $(CCOPTS) -o $(OUT)/usr trc.c usr.c 
	$(VLG) $(OUT)/usr

str:
	$(CC) -DRUN_TESTS_STR $(CCOPTS) -o $(OUT)/str trc.c str.c
	$(VLG) $(OUT)/str

#//:~

