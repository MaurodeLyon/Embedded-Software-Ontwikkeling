#include <stdio.h>

/* count lines in input*/
void main()
{
	int c;
	while ((c = getchar()) != EOF) {
		if (c == "  ") c = ' ';
	}
	printf("%s\n", c);
}