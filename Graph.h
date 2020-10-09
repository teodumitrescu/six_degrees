#ifndef GRAPH_H_
#define GRAPH_H_
#include "Util.h"

typedef struct TNode{
	int v; //numarul nodului
	struct TNode *next;
}TNode, *ATNode;

typedef struct{
	int nn; //numarul de noduri din graf
	ATNode *adl; //lista de adiacenta
}TGraphL;

TGraphL* createGraphAdjList(int numberOfNodes){

	TGraphL *list = (TGraphL *)calloc(1,sizeof(TGraphL));
	list->nn = numberOfNodes;
	list->adl = (TNode **)calloc(numberOfNodes,sizeof(TNode *));
	return list;
}

void addEdgeList(TGraphL* graph, int v1, int v2){
	TNode *node1 = (TNode *)calloc(1,sizeof(TNode));
	TNode *node2 = (TNode *)calloc(1,sizeof(TNode));
	node1->v = v1;
	node2->v = v2;
	node2->next = graph->adl[v1];
	graph->adl[v1] = node2;
	node1->next = graph->adl[v2];
	graph->adl[v2] = node1;
}

void destroyGraphAdjList(TGraphL* graph){
	for( int i = 0; i< graph->nn; i++) {
		TNode *crt = graph->adl[i];
		while(crt != NULL) {
			TNode *aux = crt;
			crt = crt->next;
			free(aux);
		}
	}
	free(graph->adl);
	free(graph);
	
}

#endif /* GRAPH_H_ */
