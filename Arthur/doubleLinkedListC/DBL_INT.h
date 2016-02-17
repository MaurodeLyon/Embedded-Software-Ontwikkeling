#define _DBLINT
struct Node {
	int data;
	struct Node* next;
	struct Node* prev;
};

void InsertAtHead(int x);
void InsertAtTail(int x);
void Show();
void Clear();
int Size();

