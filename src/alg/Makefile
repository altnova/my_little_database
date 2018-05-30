#CC=gcc-8
CC=gcc
#CCOPTS=-mavx2 -ffast-math -march=native -flto -mfpmath=sse -funroll-loops -Ofast
CCOPTS=-Og -g
#VLG=/opt/valgrind/bin/valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes
VLG=
SUBDIRS=
OUT=../../bin
COMMON=../utl/trc.c ../adt/bag.c

all: tests

tests: bin stm

bin:
	$(CC) -DRUN_TESTS_BIN $(CCOPTS) -o $(OUT)/bin $(COMMON) bin.c 
	$(VLG) $(OUT)/bin

stm:
	$(CC) -DRUN_TESTS_STM $(CCOPTS) -o $(OUT)/stm $(COMMON) stm.c
	$(VLG) $(OUT)/stm ../../fxt/input.txt ../../fxt/output.txt

#//:~

