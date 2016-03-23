/* Doubly Linked List implementation */
#include<stdio.h>
#include<stdlib.h>

#include "list.h"

struct Node {			//Data stuct with pointers
	struct Node* next;	//pointer to address of next node
	struct Node* prev;	//pointer to address of previous node
	int data;			//data 
};

struct Node* head;		// global variable - pointer to head node.

//Creates a new Node and returns pointer to it. 
struct Node* GetNewNode(int x) {
	struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
	newNode->data = x;
	newNode->prev = NULL;
	newNode->next = NULL;
	return newNode;
}

//Insert at a Node
void InsertAtNode(int x) {
	struct Node* newNode = GetNewNode(x);
	if (head == NULL) {
		head = newNode;
		return;
	}
	head->prev = newNode;
	newNode->next = head;
	head = newNode;
}

//Inserts a Node at head of doubly linked list
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

//Inserts a Node at tail of Doubly linked list
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

//Prints all the elements in linked list in forward traversal order
void Print() {
	struct Node* temp = head;
	printf("Head-");
	while (temp != NULL) {
		printf("%d-", temp->data);
		temp = temp->next;
	}
	printf("Tail");
	printf("\n");
}

//Prints all elements in linked list in reverse traversal order. 
void ReversePrint() {
	struct Node* temp = head;
	if (temp == NULL) return;		// empty list, exit
					
	// Going to last Node		
	while (temp->next != NULL) {	
		temp = temp->next;			
	}
	// Traversing backward using prev pointer
	printf("Tail-");
	while (temp != NULL) {
		printf("%d-", temp->data);
		temp = temp->prev;			
	}
	printf("Head");
	printf("\n");
}