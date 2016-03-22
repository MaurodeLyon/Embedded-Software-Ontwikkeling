#include <stdio.h>

/* remove double space in input*/
main()
{
	int c, pc;
	pc = EOF;
	while ((c = getchar()) != EOF) {
		if (c == ' ') 
			if (pc != ' ')   
				putchar(c);
		if (c != ' ') 
			putchar(c);
		pc = c;
	}
	return 0;
}
