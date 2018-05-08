index:
	rm -f index
	gcc -g -o index dynarray.c binsearch.c index.c
	./index

menu:
	rm -f menu
	gcc -w -g -Werror menu2.c -o menu	

clean:
	rm -rf menu *.dSYM
