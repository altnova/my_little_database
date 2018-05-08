csv:
	gcc -g -o csv csv.c
	./csv

index:
	gcc -g -o index trace.c dynarray.c binsearch.c index.c
	./index

menu:
	rm -f menu
	gcc -w -g -Werror menu2.c -o menu	

clean:
	rm -rf csv index menu *.dSYM
