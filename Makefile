CC=gcc-8
CC=gcc
#CCOPTS=-mavx2 -ffast-math -march=native -flto -mfpmath=sse -funroll-loops -Ofast
CCOPTS=-Og -g
#VLG=/opt/valgrind/bin/valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes
VLG=

all: app

db: csv idx

tests: clean str bag tri stm rnd clk fio vec bin hsh set # fti

idx: clean
	$(CC) -DRUN_TESTS_IDX $(CCOPTS) -o bin/idx src/bin.c src/vec.c src/idx.c src/fio.c src/rec.c src/trc.c
	$(VLG) ./bin/idx

csv: clean nodatafiles
	$(CC) -DRUN_TESTS_CSV $(CCOPTS) -o bin/csv src/trc.c src/fio.c src/csv.c
	#$(VLG) ./bin/csv csv/books.csv dat/books.dat 39673
	$(VLG) ./bin/csv csv/sample.csv dat/books.dat 17

set: clean
	$(CC) -DRUN_TESTS_SET $(CCOPTS) -o bin/set src/trc.c src/bin.c src/vec.c src/set.c
	$(VLG) ./bin/set

hsh: clean
	$(CC) -DRUN_TESTS_HSH $(CCOPTS) -o bin/hsh src/vec.c src/clk.c src/rnd.c src/trc.c src/hsh.c
	$(VLG) ./bin/hsh

bin: clean
	$(CC) -DRUN_TESTS_BIN $(CCOPTS) -o bin/bin src/trc.c src/bin.c 
	$(VLG) ./bin/bin

vec: clean
	$(CC) -DRUN_TESTS_VEC $(CCOPTS) -o bin/vec src/trc.c src/vec.c 
	$(VLG) ./bin/vec

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

tri:
	$(CC) -DRUN_TESTS_TRI $(CCOPTS) -o bin/tri src/trc.c src/tri.c
	$(VLG) ./bin/tri

bag:
	$(CC) -DRUN_TESTS_BAG $(CCOPTS) -o bin/bag src/trc.c src/bag.c
	$(VLG) ./bin/bag

str:
	$(CC) -DRUN_TESTS_STR $(CCOPTS) -o bin/str src/hsh.c src/trc.c src/str.c
	$(VLG) ./bin/str

vim:
	$(CC) -DRUN_TESTS_VIM $(CCOPTS) -o bin/vim src/vec.c src/trc.c src/vim.c
	$(VLG) ./bin/vim

fti:
	$(CC) -DRUN_TESTS_FTI $(CCOPTS) -o bin/fti src/set.c src/rnd.c src/bag.c src/tri.c src/usr.c src/stm.c src/clk.c src/bin.c src/vec.c src/idx.c src/rec.c src/trc.c src/hsh.c src/fio.c src/fti.c 
	$(VLG) ./bin/fti

app: 
	$(CC) $(CCOPTS) -o bin/cli src/set.c src/cli.c src/rnd.c src/bag.c src/tri.c src/usr.c src/stm.c src/clk.c src/bin.c src/vec.c src/idx.c src/rec.c src/trc.c src/hsh.c src/fio.c src/fti.c 
	$(VLG) ./bin/cli

clean:
	mkdir -p bin
	rm -rf *.dSYM bin/*.dSYM

nodatafiles:
	rm -f dat/books.*

#:~

