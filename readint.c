#include <stdio.h>

int input_number(int *number)
{
    char line[4096];

    while (printf("Коль ну чо: ") > 0 && fgets(line, sizeof(line), stdin) != 0)
    {
        if (sscanf(line, "%d", number) == 1)
            return 0;
        printf("Коленька ну что же ты\n");
    }
    return EOF;
}

int main(void)
{

	int n;
	input_number(&n);
	printf("Коля вот твои %d пачки гандонов\n", n);

} 