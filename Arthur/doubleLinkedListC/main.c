#include <stdio.h>

#include "DBL_INT.c";

int main()
{
	int idx;	

	InsertAtHead(1);
	InsertAtHead(2);
	InsertAtHead(3);
	show();
	Remove(2);
	show();

	/*for(idx = 0; idx < 10; idx++)
	{
		add(idx);
	}	*/	
	

	return 1;
}
