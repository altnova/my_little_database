#CC=gcc-8
CC=gcc
#CCOPTS=-mavx2 -ffast-math -march=native -flto -mfpmath=sse -funroll-loops -Ofast
CCOPTS=-Og -g
#VLG=/opt/valgrind/bin/valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes
VLG=
OUT=../../bin

all: tests

tests: msg

srv:
	$(CC) -DRUN_TESTS_SRV $(CCOPTS) -Wno-parentheses -o $(OUT)/srv ../rpc/rpc.c ../utl/trc.c msg.c tcp.c srv.c
	$(VLG) $(OUT)/srv

cln:
	$(CC) -DRUN_TESTS_CLN $(CCOPTS) -Wno-parentheses -o $(OUT)/cln ../rpc/rpc.c ../utl/trc.c tcp.c msg.c cln.c
	$(VLG) $(OUT)/cln

msg:
	$(CC) -DRUN_TESTS_MSG $(CCOPTS) -Wno-parentheses -o $(OUT)/msg ../rpc/rpc.c ../utl/str.c ../utl/trc.c tcp.c msg.c
	$(VLG) $(OUT)/msg


#//:~

