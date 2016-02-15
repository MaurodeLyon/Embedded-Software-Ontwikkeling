#include <string.h>
#include <stdio.h>


void main()
{
	int i, c,dex;
	int character[128];

	for (i = 0; i < 128; i++)
	{
		character[i] = 0;
	}

	while ((c = getchar()) != EOF)
	{
		character[c]++;
	}
		

		for (i = 0; i < 128; i++)
		{
			putchar(i);
			putchar('-');
			for (dex = 0; dex < character[i]; dex++)
			{
				putchar('*');
			}

			putchar('\n');


		}
		
		
	


}