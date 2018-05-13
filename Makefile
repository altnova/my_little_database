all: clean clk rnd bin arr fio csv idx
	# hsh
	echo "done"

csv: clean nodatafiles
	gcc -DRUN_TESTS_CSV -g -o bin/csv src/trc.c src/csv.c
	./bin/csv csv/sample.csv dat/books.dat

hsh: clean
	gcc -DRUN_TESTS_HSH -g -o bin/hsh src/clk.c src/rnd.c src/trc.c src/hsh.c
	./bin/hsh

idx: clean
	gcc -DRUN_TESTS_IDX -g -o bin/idx src/bin.c src/arr.c src/idx.c src/fio.c src/rec.c src/trc.c
	./bin/idx

menu: clean
	gcc -w -Werror -g -o bin/menu src/scr.c
	./bin/menu

bin: clean
	gcc -DRUN_TESTS_BIN -g -o bin/binsearch src/trc.c src/bin.c 
	./bin/binsearch

arr: clean
	gcc -DRUN_TESTS_ARR -g -o bin/arr src/trc.c src/arr.c 
	./bin/arr

fio:
	gcc -DRUN_TESTS_FIO -g -o bin/fio src/trc.c src/fio.c 
	./bin/fio

clk:
	gcc -DRUN_TESTS_CLK -O0 -g -o bin/clk src/trc.c src/clk.c 
	./bin/clk

rnd:
	gcc -DRUN_TESTS_RND -g -o bin/rnd src/trc.c src/rnd.c 
	./bin/rnd

tok:
	gcc -DRUN_TESTS_TOK -g -o bin/tok src/trc.c src/hsh.c src/fio.c src/tok.c 
	./bin/tok

usr:
	gcc -DRUN_TESTS_USR -g -o bin/usr src/trc.c src/usr.c 
	./bin/usr

clean:
	mkdir -p bin
	rm -rf *.dSYM bin/*.dSYM

nodatafiles:
	rm -f dat/books.*

