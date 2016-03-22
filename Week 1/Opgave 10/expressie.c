#include <stdio.h>

#define ESC_CHAR '\\'

int main(void)
{
	int c;

	while ((c = getchar()) != EOF)
	{
		switch (c)
		{
		case '\b':
			putchar(ESC_CHAR);
			putchar('b');
			break;
		case '\t':
			putchar(ESC_CHAR);
			putchar('t');
			break;
		case ESC_CHAR:
			putchar(ESC_CHAR);
			putchar(ESC_CHAR);
			break;
		default:
			putchar(c);
			break;
		}
	}
	return 0;
}
