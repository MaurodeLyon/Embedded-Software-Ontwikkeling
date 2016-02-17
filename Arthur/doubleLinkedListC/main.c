#include <stdio.h>
#include "DBL_INT.h";

int main()
{
	int idx;	

	InsertAtHead(1);
	InsertAtHead(2);
	InsertAtHead(3);
	printf("%d \n", Size());
	Show();

	printf("-----\n");
	Remove(2);
	Show();
	Clear();
	printf("-----\n");
	Show();
	printf("-----\n");
	printf("%d \n", Size());
	printf("-----\n");
	

	/*for(idx = 0; idx < 10; idx++)
	{
		add(idx);
	}	*/	
	

	return 1;
}
