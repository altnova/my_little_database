csv:
	gcc -g -o csv csv.c
	./csv

index:
	gcc -g -o idx dynarray.c binsearch.c rec.c io.c trc.c idx.c
	./idx

menu:
	rm -f menu
	gcc -w -g -Werror menu2.c -o menu	

clean:
	rm -rf csv index menu *.dSYM
