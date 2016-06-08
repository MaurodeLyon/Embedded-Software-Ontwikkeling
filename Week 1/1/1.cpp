#include <stdio.h>
int smile_bmp[] =
{
	0b00111100,
	0b01000010,
	0b10100101,
	0b10000001,
	0b10100101,
	0b10011001,
	0b01000010,
	0b00111100
};


void draw(int *data)
{
	printf("data 0 : %d\n", data[0]);
	printf("size of data : %d\n", sizeof(data));
	printf("size of data 0: %d\n", sizeof(data[0]));
	for (int i = 0; i < 8; i = i + 1)
	{
		printf("%d\n",data[i]);	// data
	}
}

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
	int * ps;
	ps = smile_bmp;
	draw(ps);
}