csv:
	gcc -g -o csvp src/csv.c
	./csvp

index:
	gcc -g -o idx src/bin.c src/arr.c src/idx.c src/fio.c src/rec.c src/trc.c
	./idx

menu:
	rm -f menu
	gcc -w -g -Werror menu2.c -o menu	

clean:
	rm -rf csv index menu *.dSYM
