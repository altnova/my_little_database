#CC=gcc-8
CC=gcc
#CCOPTS=-mavx2 -ffast-math -march=native -flto -mfpmath=sse -funroll-loops -Ofast
CCOPTS=-Og -g
#VLG=/opt/valgrind/bin/valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes
VLG=
SUBDIRS=

all: app

db: csv idx

dep:
	make -C src/adt
	make -C src/alg
	make -C src/rpc
	make -C src/net

srv:
	make -C src/net srv

cln:
	make -C src/net cln

tests: clean str stm rnd clk fio bin # fti

idx: clean
	$(CC) -DRUN_TESTS_IDX $(CCOPTS) -o bin/idx src/bin.c src/adt/vec.c src/idx.c src/fio.c src/rec.c src/trc.c
	rm -f dat/test.dat dat/test.idx
	$(VLG) ./bin/idx

csv: clean nodatafiles
	$(CC) -DRUN_TESTS_CSV $(CCOPTS) -o bin/csv src/trc.c src/fio.c src/csv.c
	$(VLG) ./bin/csv csv/books.csv dat/books.dat 39673
	#$(VLG) ./bin/csv csv/sample.csv dat/books.dat 17

bin:
	$(CC) -DRUN_TESTS_BIN $(CCOPTS) -o bin/bin src/trc.c src/bin.c 
	$(VLG) ./bin/bin

fio:
	$(CC) -DRUN_TESTS_FIO $(CCOPTS) -o bin/fio src/trc.c src/fio.c 
	$(VLG) ./bin/fio

clk:
	$(CC) -DRUN_TESTS_CLK $(CCOPTS) -o bin/clk src/trc.c src/clk.c 
	$(VLG) ./bin/clk

rnd:
	$(CC) -DRUN_TESTS_RND $(CCOPTS) -o bin/rnd src/trc.c src/rnd.c 
	$(VLG) ./bin/rnd

usr:
	$(CC) -DRUN_TESTS_USR $(CCOPTS) -o bin/usr src/trc.c src/usr.c 
	$(VLG) ./bin/usr

stm:
	$(CC) -DRUN_TESTS_STM $(CCOPTS) -o bin/stm src/trc.c src/stm.c
	$(VLG) ./bin/stm

str:
	$(CC) -DRUN_TESTS_STR $(CCOPTS) -o bin/str src/adt/hsh.c src/trc.c src/str.c
	$(VLG) ./bin/str

vim:
	$(CC) -DRUN_TESTS_VIM $(CCOPTS) -o bin/vim src/adt/vec.c src/trc.c src/vim.c
	$(VLG) ./bin/vim

fts:
	$(CC) -DRUN_TESTS_FTS $(CCOPTS) -o bin/fts src/trc.c src/adt/set.c src/adt/vec.c src/adt/hsh.c src/adt/bag.c src/clk.c src/bin.c src/idx.c src/fio.c src/stm.c src/mem.c src/fti.c src/fts.c
	$(VLG) ./bin/fts

fti:
	$(CC) -DRUN_TESTS_FTI $(CCOPTS) -o bin/fti src/mem.c src/adt/set.c src/rnd.c src/adt/bag.c src/usr.c src/stm.c src/clk.c src/bin.c src/adt/vec.c src/idx.c src/rec.c src/trc.c src/adt/hsh.c src/fio.c src/fti.c 
	$(VLG) ./bin/fti

app: 
	$(CC) $(CCOPTS) -o bin/cli src/fts.c src/mem.c src/adt/set.c src/cli.c src/rnd.c src/adt/bag.c src/usr.c src/stm.c src/clk.c src/bin.c src/adt/vec.c src/idx.c src/rec.c src/trc.c src/adt/hsh.c src/fio.c src/fti.c 
	$(VLG) ./bin/cli

clean:
	mkdir -p bin
	rm -rf *.dSYM bin/*.dSYM

nodatafiles:
	rm -f dat/books.*

#//:~

