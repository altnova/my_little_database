csv:
	gcc -g -o csvp src/csv.c
	./csvp

index:
	gcc -g -o idx src/binsearch.c src/dynarray.c src/idx.c src/io.c src/rec.c src/trc.c
	./idx

menu:
	rm -f menu
	gcc -w -g -Werror menu2.c -o menu	

clean:
	rm -rf csv index menu *.dSYM
