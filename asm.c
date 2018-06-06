#include <stdio.h> 
#include <stdlib.h>

/*
 * 	дан массив А из 16 байтов. последовательно разбить на слова. если в слове 
 * 	старший байт меньше младшего (знаковые), заменить его нулем, предварительно
 * 	скопировав в массив B. в массив С поместить адрес (смещение этого слова).
 *	сосчитать количество таких слов
 */

void debug()
{
	printf("boom\n");
	fflush(stdout);
}

char a_asm(unsigned char *a, unsigned char *b, char *c) 
{
	char i;
	asm {
		mov rsi, qword ptr a
		mov rbx, qword ptr b
		mov rdx, qword ptr c
		xor cx, cx
	
		loop:
		mov ax, word ptr [rsi]
		cmp al, ah

		//JBE aaa                         // (unsigned)
		jle aaa					        // (signed) если старший больше или равен, прыгаем в aaa
		mov byte ptr [rbx], ah 			// сохранить старший байт в b
		xor ax, ax						// обнулить слово
		mov byte ptr [rsi+1], ah 			// запихать ноль в адрес
		mov qword ptr [rdx], rsi 		// сохранить адрес слова
		add rdx,8
		inc ch 
		inc rbx

		aaa:
		add rsi, 2
		inc cl
		cmp cl, 8
		jl loop 

		mov i, ch
	}
	return i;
}

int main()
{
	
	unsigned char *a;
	unsigned char *b;
	char **c;
	char i, j;

	a = malloc(16 * sizeof(char));
	b = calloc(8, sizeof(char));
	c = malloc(8 * sizeof(char*));


	//for (j = 0; j < 16; j++) 
	//	scanf("%hhx", &a[j]);

	//char *s= "ca cc 1 ca c a 9 99 98 ab b a 12 12 12 11";
	char *s = "a f d f ff d a c d e f f cc e d a";

	sscanf(s,
		"%02hhX %02hhX %02hhX %02hhX %02hhX %02hhX %02hhX %02hhX \
		 %02hhX %02hhX %02hhX %02hhX %02hhX %02hhX %02hhX %02hhX",
		&a[0], &a[1], &a[2], &a[3], &a[4], &a[5], &a[6], &a[7], &a[8],
		&a[9], &a[10], &a[11], &a[12], &a[13], &a[14], &a[15]);
	
	for (j = 0; j < 16; j++) {
		printf("%#2x ", a[j]);
	} printf("\n");


	i = a_asm(a, b, c);

	printf("%d numbers\nA: ", i);
	for (j = 0; j < 16; ++j) 
		printf("%#2x ", a[j]);

	printf("\nB: ");
	for (j = 0; j < i; ++j) 
		printf("%#2x ", b[j]);

	printf("\nC: ");
	for (j = 0; j < i; ++j) 
		printf("%p ", 	c[j]);
	printf("\n");

	free(a);
	free(b);
	free(c);
	return 0;
}