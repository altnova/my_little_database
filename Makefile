#CC=gcc-8
CC=gcc
#CCOPTS=-lm -mavx2 -ffast-math -march=native -flto -mfpmath=sse -funroll-loops -Ofast
CCOPTS=-Og -g -lm
#VLG=/opt/valgrind/bin/valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes
VLG=
COMMON=src/utl/trc.c src/adt/bag.c

all: nsr

db: nodb csv
	$(VLG) ./bin/csv csv/books.csv dat/books.dat 39673

testdb: notestdb csv 
	$(VLG) ./bin/csv csv/reference.csv fxt/reference.dat 17

idx: testdb
	$(CC) -DRUN_TESTS_IDX $(CCOPTS) -o bin/idx $(COMMON) src/mem.c src/adt/hsh.c src/alg/bin.c src/adt/vec.c src/idx.c src/utl/fio.c src/rec.c
	./bin/idx # will use fxt/reference.dat fxt/reference.idx

csv:
	$(CC) -DRUN_TESTS_CSV $(CCOPTS) -o bin/csv $(COMMON) src/utl/fio.c src/csv.c

fti:
	$(CC) -DRUN_TESTS_FTI $(CCOPTS) -o bin/fti $(COMMON) src/mem.c src/adt/set.c src/utl/rnd.c src/utl/usr.c src/alg/stm.c src/utl/clk.c src/alg/bin.c src/adt/vec.c src/idx.c src/rec.c src/adt/hsh.c src/utl/fio.c src/fti.c 
	$(VLG) ./bin/fti

fts:
	$(CC) -DRUN_TESTS_FTS $(CCOPTS) -o bin/fts $(COMMON) src/rec.c src/adt/set.c src/adt/vec.c src/adt/hsh.c src/utl/clk.c src/alg/bin.c src/idx.c src/utl/fio.c src/alg/stm.c src/mem.c src/fti.c src/fts.c
	$(VLG) ./bin/fts

nsr:
	$(CC) -DRUN_TESTS_NSR $(CCOPTS) -Wno-parentheses -o bin/nsr $(COMMON) src/idx.c src/rec.c src/fts.c src/fti.c src/adt/vec.c src/adt/set.c src/adt/hsh.c src/alg/bin.c src/alg/stm.c src/mem.c src/utl/clk.c src/utl/fio.c src/net/srv.c src/net/tcp.c src/net/msg.c src/rpc/rpc.c src/nsr.c
	./bin/nsr 5000

ncl:
	$(CC) -DNET_CLIENT $(CCOPTS) -Wno-parentheses -o bin/ncl $(COMMON) src/rec.c src/cli.c src/rpc/rpc.c src/net/tcp.c src/net/msg.c src/net/cln.c src/ncl.c
	$(VLG) ./bin/ncl localhost 5000

cli: 
	$(CC) -DCLI_STANDALONE $(CCOPTS) -o bin/cli $(COMMON) src/fts.c src/mem.c src/adt/set.c src/utl/rnd.c src/utl/usr.c src/alg/stm.c src/utl/clk.c src/alg/bin.c src/adt/vec.c src/idx.c src/rec.c src/adt/hsh.c src/utl/fio.c src/fti.c src/cli.c
	$(VLG) ./bin/cli

dep: clean
	make -C src/adt
	make -C src/alg
	make -C src/utl
	#make -C src/rpc # requires gnu preprocessor
	make -C src/net

vim:
	$(CC) -DRUN_TESTS_VIM $(CCOPTS) -o bin/vim $(COMMON) src/adt/vec.c src/vim.c
	$(VLG) ./bin/vim

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

