#CC=gcc-8
CC=gcc
#CCOPTS=-lm -mavx2 -ffast-math -march=native -flto -mfpmath=sse -funroll-loops -Ofast
CCOPTS=-Og -g -lm
#VLG=/opt/valgrind/bin/valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes
VLG=
COMMON=src/utl/trc.c src/adt/bag.c

all: cli

db: nodb csv
	$(VLG) ./bin/csv csv/books.csv dat/books.dat 39673

testdb: notestdb csv 
	$(VLG) ./bin/csv csv/reference.csv fxt/reference.dat 17

idx: testdb
	$(CC) -DRUN_TESTS_IDX $(CCOPTS) -o bin/idx $(COMMON) src/csv.c src/mem.c src/adt/hsh.c src/alg/bin.c src/adt/vec.c src/idx.c src/utl/fio.c src/rec.c
	./bin/idx # will use fxt/reference.dat fxt/reference.idx

csv: clean
	$(CC) -DRUN_TESTS_CSV $(CCOPTS) -o bin/csv $(COMMON) src/utl/fio.c src/csv.c

cli: clean
	#$(CC) -DCLI_STANDALONE $(CCOPTS) -o bin/cli $(COMMON) src/utl/str.c src/csv.c src/fts.c src/mem.c src/adt/set.c src/utl/rnd.c src/utl/usr.c src/alg/stm.c src/utl/clk.c src/alg/bin.c src/adt/vec.c src/idx.c src/rec.c src/adt/hsh.c src/utl/fio.c src/fti.c src/cli.c
	$(CC) -DCLI_STANDALONE $(CCOPTS) -o bin/cli $(COMMON) src/utl/str.c src/csv.c src/mem.c src/utl/usr.c src/utl/clk.c src/alg/bin.c src/adt/vec.c src/idx.c src/rec.c src/adt/hsh.c src/utl/fio.c src/cli.c
	$(VLG) ./bin/cli

clean:
	rm -rf bin
	mkdir -p bin
	touch bin/.gitdir

notestdb:
	rm -f fxt/reference.*
	rm -f fxt/tempdb.*

nodb:
	rm -f dat/books.*	

.PHONY: csv clean db nodb testdb notestdb

#//:~

