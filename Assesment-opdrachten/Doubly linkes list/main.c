/* Doubly Linked List implementation */
#include <stdio.h>
#include <stdlib.h>

#include "llist.h"


int main() {
	//creation of list
	llist_add(1);
	llist_add(2);
	llist_add(3);
	llist_add(4);
	llist_add(5);

	//display of list
	printf("----------Doubly linked list----------\n");
	printf("Head node : %d\n", head->data);

	printf("\n----------llist_show()----------\n");
	printf("entire list :\t");
	llist_show();

	printf("\n----------llist_nrItems()----------\n");
	printf("\nthere are %d items in the list\n", llist_nrItems());

	printf("\n----------llist_excist()----------\n");
	printf("Does the number 2 exist in the list %d\n", llist_excist(2));
	printf("Does the number 7 exist in the list %d\n", llist_excist(7));

	printf("\n----------llist_copy()----------\n");
	printf("the number of the head of the deepcopy is %d\n", llist_copy()->data);
	printf("the number of the second node of the deepcopy is %d\n", llist_copy()->next->data);
	printf("the number of the third node of the deepcopy is %d\n", llist_copy()->next->next->data);
	printf("the number of the fourth node of the deepcopy is %d\n", llist_copy()->next->next->next->data);
	printf("the number of the fifth node of the deepcopy is %d\n", llist_copy()->next->next->next->next->data);

	printf("\n----------llist_clear()----------\n");
	printf("clearing list\n");
	llist_clear();

	printf("\n----------llist_show()----------\n");
	printf("entire list :\t");
	llist_show();
}