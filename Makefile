#CC=gcc-8
CC=gcc
#CCOPTS=-mavx2 -ffast-math -march=native -flto -mfpmath=sse -funroll-loops -Ofast
CCOPTS=-Og -g
#VLG=/opt/valgrind/bin/valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes
VLG=
SUBDIRS=

all: app

db: csv idx

dep: clean
	make -C src/adt
	make -C src/alg
	make -C src/utl
	make -C src/rpc
	make -C src/net

srv:
	make -C src/net srv

cln:
	make -C src/net cln

idx: clean
	$(CC) -DRUN_TESTS_IDX $(CCOPTS) -o bin/idx src/alg/bin.c src/adt/vec.c src/idx.c src/utl/fio.c src/rec.c src/utl/trc.c
	rm -f dat/test.dat dat/test.idx
	$(VLG) ./bin/idx

csv: clean nodatafiles
	$(CC) -DRUN_TESTS_CSV $(CCOPTS) -o bin/csv src/utl/trc.c src/utl/fio.c src/csv.c
	$(VLG) ./bin/csv csv/books.csv dat/books.dat 39673
	#$(VLG) ./bin/csv csv/sample.csv dat/books.dat 17

fts:
	$(CC) -DRUN_TESTS_FTS $(CCOPTS) -o bin/fts src/rec.c src/utl/trc.c src/adt/set.c src/adt/vec.c src/adt/hsh.c src/adt/bag.c src/utl/clk.c src/alg/bin.c src/idx.c src/utl/fio.c src/alg/stm.c src/mem.c src/fti.c src/fts.c
	$(VLG) ./bin/fts

fti:
	$(CC) -DRUN_TESTS_FTI $(CCOPTS) -o bin/fti src/mem.c src/adt/set.c src/utl/rnd.c src/adt/bag.c src/utl/usr.c src/alg/stm.c src/utl/clk.c src/alg/bin.c src/adt/vec.c src/idx.c src/rec.c src/utl/trc.c src/adt/hsh.c src/utl/fio.c src/fti.c 
	$(VLG) ./bin/fti

app: 
	$(CC) $(CCOPTS) -o bin/cli src/fts.c src/mem.c src/adt/set.c src/utl/rnd.c src/adt/bag.c src/utl/usr.c src/alg/stm.c src/utl/clk.c src/alg/bin.c src/adt/vec.c src/idx.c src/rec.c src/utl/trc.c src/adt/hsh.c src/utl/fio.c src/fti.c src/cli.c
	$(VLG) ./bin/cli

vim:
	$(CC) -DRUN_TESTS_VIM $(CCOPTS) -o bin/vim src/adt/vec.c src/src/utl/trc.c src/vim.c
	$(VLG) ./bin/vim

clean:
	mkdir -p bin
	rm -rf *.dSYM bin/*.dSYM

nodatafiles:
	rm -f dat/books.*

#//:~

