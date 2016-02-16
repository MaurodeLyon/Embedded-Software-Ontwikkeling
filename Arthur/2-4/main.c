#include <string.h>
#include <stdio.h>


void squeeze(char s[], char c[]);

void main()
{
	squeeze("test", "t");

}


void squeeze(char s[], char c[])
{
	int i, j;

	for (i = 0; i < (sizeof(s) / sizeof(s[0])); i++)
	{
		for (j = 0; j < (sizeof(c) / sizeof(c[])); j++)
		{
			if (s[i] == c[j])
			{
				s[i] = ' ';
			}
		}

		
		printf("%s", s);


	}
}