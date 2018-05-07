menu:
	rm -f menu
	gcc -w -g -Werror menu2.c -o menu	

clean:
	rm -rf menu *.dSYM
