#include <string.h>
#include <stdio.h>

int strend(char*, char*);

void main()
{
	char stringA[10];
	char stringB[10];


	strcpy(stringA, "heinz");
	strcpy(stringB, "z");

	printf("%i\n",strend(stringA, stringB));

	
}

int strend(char* s, char* t)
{
	int sizeA;
	char lastA;


	sizeA = strlen(s);
	lastA = s[sizeA-1];
	//printf("%c\n",lastA);

	if (t[0] == lastA)
	{
		return 1;
	}
	else {
		return 0;
	}

}






