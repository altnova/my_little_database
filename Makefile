csv: clean nodatafiles
	gcc -g -o bin/csv src/trc.c src/csv.c
	./bin/csv

idx: clean
	gcc -g -o bin/idx src/bin.c src/arr.c src/idx.c src/fio.c src/rec.c src/trc.c
	./bin/idx

menu: clean
	gcc -w -Werror -g -o bin/menu src/scr.c
	./bin/menu

bin: clean
	gcc -DRUN_TESTS -g -o bin/binsearch src/bin.c 
	./bin/binsearch

arr: clean
	gcc -DRUN_TESTS -g -o bin/dynarray src/trc.c src/arr.c 
	./bin/dynarray

clean:
	mkdir -p bin
	rm -rf *.dSYM

nodatafiles:
	rm -f dat/books.*

