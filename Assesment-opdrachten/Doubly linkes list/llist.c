#include<stdio.h>
#include<stdlib.h>

#include "llist.h"

//maak een nieuwe node aan
struct Node* createNewNode(int data) {
	struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
	newNode->data = data;
	newNode->prev = NULL;
	newNode->next = NULL;
	return newNode;
}

// print de gehele lijst
void llist_show(void) {
	struct Node* temp = head;
	printf("Head-");
	while (temp != NULL) {
		printf("%d-", temp->data);
		temp = temp->next;
	}
	printf("Tail");
	printf("\n");
}

// voeg item toe aan lijst
void llist_add(int data) {
	struct Node* temp = head;
	struct Node* newNode = createNewNode(data);
	if (head == NULL) {
		head = newNode;
		return;
	}
	while (temp->next != NULL) temp = temp->next; // Go To last Node
	temp->next = newNode;
	newNode->prev = temp;
}

// remove item uit lijst
void llist_remove(int position) {
	int increment = 1;
	struct Node* selectedNode = head;

	//look for correct node
	while (increment < position) {
		if (selectedNode->next == NULL) {
			printf("llist_remove: Index out of bounds error.\n");
			return;
		}
		selectedNode = selectedNode->next;
		increment++;
	}

	//if position is <= 1
	if (selectedNode->prev == NULL) {
		head = selectedNode->next;
		head->prev = NULL;
	}

	//if selectednode doesnt have next
	if (selectedNode->next == NULL) {
		selectedNode->prev->next = NULL;
	}
	free(selectedNode);
}

// clear de gehele lijst
void llist_clear(void) {
	struct Node* nodeToRemove;
	while (head->next != NULL)
	{
		nodeToRemove = head;
		head = head->next;
		free(nodeToRemove);
	}
	free(head);
	head = NULL;
}

// bepaal aantal elementen in lijst
int  llist_nrItems(void) {
	if (head == NULL)return 0;

	int amount = 1;
	struct Node* next;
	next = head;
	while (next->next != NULL) {
		next = next->next;
		amount++;
	}
	return amount;
}

// Bepaal of een element in de lijst voorkomt
int  llist_excist(int x) {
	if (head == NULL)return 0;

	struct Node* next;
	next = head;
	while (next->next != NULL) {
		if (next->data == x)return 1;
		next = next->next;
	}
	return 0;
}

// maak ‘deep copy’ (moeilijk!)
struct Node*  llist_copy(void) {	
	if (head == NULL)return 0;
	struct Node* newHead;
	struct Node* next;
	next = head;
	
	while (next->next != NULL) {
		next = next->next;
	}
	
	newHead = createNewNode(next->data);
	while (next->prev != NULL) {
		next = next->prev;
		newHead->prev = createNewNode(next->data);
		newHead->prev->next = newHead;
		newHead = newHead->prev;
	}
	return newHead;
}
