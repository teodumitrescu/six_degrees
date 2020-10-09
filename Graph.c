#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "Util.h"
#include "Graph.h"

//functia initializeaza graful
TGraphL* createGraphAdjList(int numberOfNodes){

	TGraphL *list = (TGraphL *)calloc(1,sizeof(TGraphL));
	list->nn = numberOfNodes;
	list->adl = (TNode **)calloc(numberOfNodes,sizeof(TNode *));
	return list;
}

//functia adauga o muchie intre doua noduri in lista de adiacenta
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

//functia sterge graful format
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


