#pragma once
/*
Implementeer de volgende functionaliteit :
Print	llist_show(…)		// print de gehele lijst
		llist_add(…)		// voeg item toe aan lijst
		llist_remove(…)		// remove item uit lijst
		llist_clear(…)		// clear de gehele lijst
		llist_nrItems(…)	// bepaal aantal elementen in lijst
		llist_excist(…)		// Bepaal of een element in de lijst voorkomt
		llist_copy(…)		// maak ‘deep copy’ (moeilijk!)
*/
struct Node {			//Data stuct with pointers
	struct Node* next;	//pointer to address of next node
	struct Node* prev;	//pointer to address of previous node
	int data;			//data 
};

struct Node* head;// global variable - pointer to head node.

//Insert at a Node
void InsertAtNode(int x);

//Inserts a Node at head of doubly linked list
void InsertAtHead(int x);

//Inserts a Node at tail of Doubly linked list
void InsertAtTail(int x);

//Prints all the elements in linked list form head to tail
void Print();

//Prints all elements in linked list from tail to head
void ReversePrint();