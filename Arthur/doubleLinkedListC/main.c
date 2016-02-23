#include <stdio.h>
#include "DBL_INT.h";

int main()
{
	int idx;	

	InsertAtHead(1);
	InsertAtHead(2);
	InsertAtHead(3);
	printf("Size: %d \n", Size());
	Show();

	printf("-----\n");
	Remove(2);
	printf("Size: %d \n", Size());
	Show();
	printf("-----\n");
	printf("Does '3' exist in the list?:%d \n", Exist(3));
	printf("-----\n");
	Clear();
	Show();
	printf("-----\n");
	

	/*for(idx = 0; idx < 10; idx++)
	{
		add(idx);
	}	*/	
	

	return 1;
}
