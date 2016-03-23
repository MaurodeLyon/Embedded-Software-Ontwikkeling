#include <stdio.h>
#include<stdlib.h>
#include"DBL_INT.h"

//struct Node GetNewMode(int);
//void InsertAtHead(int x);
//void InsertAtTail(int x);
//void show();

//head of the list
struct Node* head; 

//Allocate a new node for use. You can also give a value to be put in the new node.
struct Node* GetNewNode(int x) {
	struct Node* newNode
		= (struct Node*)malloc(sizeof(struct Node));
	newNode->data = x;
	newNode->prev = NULL;
	newNode->next = NULL;
	return newNode;
}

//Insert a new node at the head of the list
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

//Put a new node at the tail of the list.
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

//remove an instance of the given value. Remove the return; part to remove all instances of the given object.
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


//Show all values in the list
void Show()
{
	struct Node *p = head;
	int nr = 0;
	if (head != NULL)
	{
		for (; p->next != NULL; p = p->next)
		{
			printf("%d - [%d]\n", nr, p->data);
			nr++;
		}
		
		printf("%d - [%d]\n", nr++, p->data);
	}
	else
	{
		printf("Current list is empty \n");
	}
	
}

//Clear the whole list
void Clear()
{
	struct Node *p = head;
	struct Node *prev;

	while (p->next != NULL) p = p->next;

	for (; p->prev != NULL;)
	{
		prev = p->prev;
		free(p);
		p = prev;
	}

	head = NULL;
}

//Return the size of the current list
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
//Check if the value exists in the current list.
//Return 1 if found, 0 if not
int Exist(int x)
{
	struct Node *p = head;
	int nr = 0;

	for (; p->next != NULL; p = p->next)
		if (p->data == x)
		{
			return 1;
			
		}

	return 0;

}

//Hardcopy the entire list and return a node as access point
struct Node* Copy()
{
	struct Node *p = head;
	struct Node *tempHead = GetNewNode(p->data);
	p = p->next;
	

	for (; p->next != NULL; p->next)
	{
		tempHead = tempHead->next;
		tempHead = GetNewNode(p->data);


	}

	return tempHead;
}

	


