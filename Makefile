CC=gcc-8
#CC=gcc
VLG=/opt/valgrind/bin/valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes
#--verbose --log-file=valgrind-out.txt
VLG=

all: clean bag tri stm rnd clk fio vec bin hsh

db: csv idx

idx: clean
	$(CC) -DRUN_TESTS_IDX -O2 -g -o bin/idx src/bin.c src/vec.c src/idx.c src/fio.c src/rec.c src/trc.c
	$(VLG) ./bin/idx

csv: clean nodatafiles
	$(CC) -DRUN_TESTS_CSV -O2 -o bin/csv src/trc.c src/csv.c
	$(VLG) ./bin/csv csv/books.csv dat/books.dat

set: clean
	$(CC) -DRUN_TESTS_SET -g -o bin/set src/trc.c src/hsh.c src/bag.c src/set.c
	$(VLG) ./bin/set

hsh: clean
	$(CC) -DRUN_TESTS_HSH -O2 -g -o bin/hsh src/vec.c src/clk.c src/rnd.c src/trc.c src/hsh.c
	$(VLG) ./bin/hsh

bin: clean
	$(CC) -DRUN_TESTS_BIN -O2 -g -o bin/binsearch src/trc.c src/bin.c 
	$(VLG) ./bin/binsearch

vec: clean
	$(CC) -DRUN_TESTS_VEC -O2 -g -o bin/vec src/trc.c src/vec.c 
	$(VLG) ./bin/vec

fio:
	$(CC) -DRUN_TESTS_FIO -O2 -g -o bin/fio src/trc.c src/fio.c 
	$(VLG) ./bin/fio

clk:
	$(CC) -DRUN_TESTS_CLK -O2 -O0 -g -o bin/clk src/trc.c src/clk.c 
	$(VLG) ./bin/clk

rnd:
	$(CC) -DRUN_TESTS_RND -O2 -g -o bin/rnd src/trc.c src/rnd.c 
	$(VLG) ./bin/rnd

usr:
	$(CC) -DRUN_TESTS_USR -O2 -g -o bin/usr src/trc.c src/usr.c 
	$(VLG) ./bin/usr

stm:
	$(CC) -DRUN_TESTS_STM -O2 -g -o bin/stm src/trc.c src/stm.c
	$(VLG) ./bin/stm

tri:
	$(CC) -DRUN_TESTS_TRI -O2 -g -o bin/tri src/trc.c src/tri.c
	$(VLG) ./bin/tri

bag:
	$(CC) -DRUN_TESTS_BAG -O2 -g -o bin/bag src/trc.c src/bag.c
	$(VLG) ./bin/bag

tok:
	$(CC) -DRUN_TESTS_TOK -g -o bin/tok src/rnd.c src/bag.c src/tri.c src/usr.c src/stm.c src/clk.c src/bin.c src/vec.c src/idx.c src/rec.c src/trc.c src/hsh.c src/fio.c src/tok.c 
	$(VLG) ./bin/tok


clean:
	mkdir -p bin
	rm -rf *.dSYM bin/*.dSYM

nodatafiles:
	rm -f dat/books.*

