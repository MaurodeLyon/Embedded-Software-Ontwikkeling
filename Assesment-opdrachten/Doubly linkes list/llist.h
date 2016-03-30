#pragma once
/*
Implementeer de volgende functionaliteit :
llist_show(…)		// print de gehele lijst
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

void llist_show(void);		// print de gehele lijst
void llist_add(int data);	// voeg item toe aan lijst
void llist_remove(int x);	// remove item uit lijst
void llist_clear(void);		// clear de gehele lijst
int  llist_nrItems(void);	// bepaal aantal elementen in lijst
int  llist_excist(int x);	// Bepaal of een element in de lijst voorkomt
struct Node* llist_copy(void);	// maak ‘deep copy’ (moeilijk!)