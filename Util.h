#ifndef UTIL_H_
#define UTIL_H_

typedef struct ListNode{
	char *actorName; //numele actorului
	int nr; //numarul nodului
	struct ListNode *next;
	struct ListNode *prev;
}ListNode;

typedef struct List{
	ListNode* head;
}List;

typedef struct Pair {
	int actor1; ///numsrul nodului primului actor din punte
	int actor2; //numarul nodului celui de-al doilea actor din punte
}Pair;

typedef List Stack;
typedef List Queue;

List* createList(void){
	List* newList = (List*) malloc(sizeof(List));
	newList->head = (ListNode*) malloc(sizeof(ListNode));
	newList->head->next = newList->head->prev = newList->head;
	return newList;
}
void destroyList(List* list){
	ListNode* it = list->head->next;
	while(it != list->head){
		ListNode* aux = it;
		it = it->next;
		free(aux);
	}
	free(list->head);
	free(list);
}
int isListEmpty(List *list){
	return list->head->next == list->head;
}

void push(Stack* stack, int key){
	ListNode* newNode = (ListNode*) malloc(sizeof(ListNode));
	newNode->nr = key;

	newNode->next = stack->head->next;
	newNode->prev = stack->head;

	stack->head->next->prev = newNode;
	stack->head->next = newNode;
}

Queue* createQueue(void){
	return createList();
}
void destroyQueue(Queue* queue){
	destroyList(queue);
}
void enqueue(Queue* queue, int key){
	push(queue,key);
}
void dequeue(Queue* queue){
	ListNode* aux = queue->head->prev;
	queue->head->prev = queue->head->prev->prev;
	queue->head->prev->next = queue->head;
	free(aux);
}

int isQueueEmpty(Queue *queue){
	return isListEmpty(queue);
}

int front(Queue* queue){
	return queue->head->prev->nr;
}

#endif /* UTIL_H_ */
