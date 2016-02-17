#ifndef LLIST_DEF
#define LLIST_DEF

struct node
{
	int data;
	struct node *next;
	struct node *prev;
};

void init();
int add(int data);
void show();

#endif
