all: clean bin arr fio hsh csv idx
	echo "done"

csv: clean nodatafiles
	gcc -DRUN_TESTS -g -o bin/csv src/trc.c src/csv.c
	./bin/csv csv/sample.csv dat/books.dat

hsh: clean
	gcc -DRUN_TESTS -g -o bin/hsh src/trc.c src/hsh.c
	./bin/hsh

idx: clean
	gcc -DRUN_TESTS_IDX -g -o bin/idx src/bin.c src/arr.c src/idx.c src/fio.c src/rec.c src/trc.c
	./bin/idx

menu: clean
	gcc -w -Werror -g -o bin/menu src/scr.c
	./bin/menu

bin: clean
	gcc -DRUN_TESTS -g -o bin/binsearch src/trc.c src/bin.c 
	./bin/binsearch

arr: clean
	gcc -DRUN_TESTS -g -o bin/dynarray src/trc.c src/arr.c 
	./bin/dynarray

fio:
	gcc -DRUN_TESTS -g -o bin/fio src/trc.c src/fio.c 
	./bin/fio

clk:
	gcc -DRUN_TESTS -O0 -g -o bin/clk src/trc.c src/clk.c 
	./bin/clk

clean:
	mkdir -p bin
	rm -rf *.dSYM bin/*.dSYM

nodatafiles:
	rm -f dat/books.*

