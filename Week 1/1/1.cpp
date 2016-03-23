#include <stdio.h>
void main()
{
	printf("hello, ");
	printf("world");
	printf("\n");

	int a = 5;
	int *pa;
	pa = &a;
	printf("the value of a    :\t%d\n", a);
	printf("the address of a  :\t%d\n", &a);
	printf("pointer to address:\t%d\n", pa);
	printf("address of pointer:\t%d\n", &pa);
	printf("dereference of pa :\t%d\n", *pa);
}