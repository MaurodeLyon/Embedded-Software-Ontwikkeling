#include <stdio.h>
#include<stdlib.h>
#include"DBL_INT.h"

//struct Node GetNewMode(int);
//void InsertAtHead(int x);
//void InsertAtTail(int x);
//void show();


struct Node* head; 

struct Node* GetNewNode(int x) {
	struct Node* newNode
		= (struct Node*)malloc(sizeof(struct Node));
	newNode->data = x;
	newNode->prev = NULL;
	newNode->next = NULL;
	return newNode;
}

//Links
void InsertAtHead(int x) {
	struct Node* newNode = GetNewNode(x);
	if (head == NULL) {
		head = newNode;
		return;
	}
	head->prev = newNode;
	newNode->next = head;
	head = newNode;
}
//Rechts

void InsertAtTail(int x) {
	struct Node* temp = head;
	struct Node* newNode = GetNewNode(x);
	if (head == NULL) {
		head = newNode;
		return;
	}
	while (temp->next != NULL) temp = temp->next; // Go To last Node
	temp->next = newNode;
	newNode->prev = temp;
}

void Remove(int object)
{
	struct Node *p = head;
	int nr = 0;

	for (; p->next != NULL; p = p->next)
		if (p->data == object)
		{
			p->prev->next = p->next;
			p->next->prev = p->prev;
			free(p);
			return;
		}
	
}

void Show()
{
	struct Node *p = head;
	int nr = 0;

	for (; p->next!=NULL; p = p->next)
	{
		printf("%d - [%d]\n", nr, p->data);
		nr++;
	}

	printf("%d - [%d]\n", nr++, p->data);
}

void Clear()
{
	struct Node *p = head;
	
	/*for (; NULL != p->next; p = p->next)
	{
		
		


	}*/

	

	

}

int Size()
{
	struct Node *temp = head;
	int size = 1;

	if(head==NULL)
	{
		return 0;
	}

	while (temp->next != NULL)
	{
		temp = temp->next;
		size++;
	}
	
	return size;
}

int Exist(int x)
{

}

	


