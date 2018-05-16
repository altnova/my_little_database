#CC=gcc-7
CC=gcc

all: clean hea tri stm rnd clk fio arr bin hsh

db:
	csv idx

menu: clean
	$(CC) -w -Werror -g -o bin/menu src/scr.c
	./bin/menu

idx: clean
	$(CC) -DRUN_TESTS_IDX  -O2 -g -o bin/idx src/bin.c src/arr.c src/idx.c src/fio.c src/rec.c src/trc.c
	./bin/idx

csv: clean nodatafiles
	$(CC) -DRUN_TESTS_CSV  -O2 -g -o bin/csv src/trc.c src/csv.c
	./bin/csv csv/books.csv dat/books.dat

hsh: clean
	$(CC) -DRUN_TESTS_HSH -O2 -g -o bin/hsh src/arr.c src/clk.c src/rnd.c src/trc.c src/hsh.c
	./bin/hsh

bin: clean
	$(CC) -DRUN_TESTS_BIN -O2 -g -o bin/binsearch src/trc.c src/bin.c 
	./bin/binsearch

arr: clean
	$(CC) -DRUN_TESTS_ARR -O2 -g -o bin/arr src/trc.c src/arr.c 
	./bin/arr

fio:
	$(CC) -DRUN_TESTS_FIO -O2 -g -o bin/fio src/trc.c src/fio.c 
	./bin/fio

clk:
	$(CC) -DRUN_TESTS_CLK -O2 -O0 -g -o bin/clk src/trc.c src/clk.c 
	./bin/clk

rnd:
	$(CC) -DRUN_TESTS_RND -O2 -g -o bin/rnd src/trc.c src/rnd.c 
	./bin/rnd

usr:
	$(CC) -DRUN_TESTS_USR -O2 -g -o bin/usr src/trc.c src/usr.c 
	./bin/usr

stm:
	$(CC) -DRUN_TESTS_STM -O2 -g -o bin/stm src/trc.c src/stm.c
	./bin/stm

tri:
	$(CC) -DRUN_TESTS_TRI -O2 -g -o bin/tri src/trc.c src/tri.c
	./bin/tri

hea:
	$(CC) -DRUN_TESTS_HEA -O2 -g -o bin/hea src/trc.c src/hea.c
	./bin/hea

tok:
	$(CC) -DRUN_TESTS_TOK -g -o bin/tok src/hea.c src/tri.c src/usr.c src/stm.c src/clk.c src/bin.c src/arr.c src/idx.c src/rec.c src/trc.c src/hsh.c src/fio.c src/tok.c 
	./bin/tok

clean:
	mkdir -p bin
	rm -rf *.dSYM bin/*.dSYM

nodatafiles:
	rm -f dat/books.*

