#CC=gcc-8
CC=gcc
#CCOPTS=-mavx2 -ffast-math -march=native -flto -mfpmath=sse -funroll-loops -Ofast
CCOPTS=-Og -g
#VLG=/opt/valgrind/bin/valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes
VLG=
OUT=../../bin
COMMON=../utl/trc.c ../adt/bag.c

all: tests

tests: msg

srv:
	$(CC) -DRUN_TESTS_SRV $(CCOPTS) -Wno-parentheses -o $(OUT)/srv ../utl/trc.c ../adt/bag.c ../rpc/rpc.c msg.c tcp.c srv.c
	$(VLG) $(OUT)/srv

cln:
	$(CC) -DRUN_TESTS_CLN $(CCOPTS) -Wno-parentheses -o $(OUT)/cln ../utl/trc.c ../adt/bag.c ../rpc/rpc.c tcp.c msg.c cln.c
	$(VLG) $(OUT)/cln

msg:
	$(CC) -DRUN_TESTS_MSG $(CCOPTS) -Wno-parentheses -o $(OUT)/msg ../utl/trc.c ../adt/bag.c ../rpc/rpc.c ../utl/str.c tcp.c msg.c
	$(VLG) $(OUT)/msg


#//:~

