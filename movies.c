#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <limits.h>
#include "Util.h"
#include "Graph.h"

//functia cauta un nume in hashtable si returneaza numarul nodului acestuia
int getNod(ListNode **hashtable, int hash, char *name) {

	ListNode *crt = hashtable[hash];
	while(crt != NULL) {
		if(strcmp(crt->actorName, name) == 0) {
			return crt->nr;
		}
		crt = crt->next;
	}
	return 0;
}

//functia calculeaza hashul aferent unui nume
int getHash(char *actorName, int mod) {

	int len = strlen(actorName);
	int crtHash = 0;

	//calculez hashul dupa o formula aplicata pentru fiecare litera
	for(int letter = 0; letter < len; letter++) {
		crtHash = abs((crtHash*26 + (actorName[letter] - 'a'))) % mod;
	}
	return crtHash;
}

//functa parcurge in adancime in mod recursiv nodurile componentei conexe si le
//adauga intr-un vector
void dfs(TGraphL* G, int* visited, int s, int *allVisited, int *actorsInProduction,
	long *size){
	visited[s] = 1;
	allVisited[s] = 1;
	TNode *crt = G->adl[s];
	while(crt != NULL) {
		if(visited[crt->v] == 0) {
			actorsInProduction[*size]  = crt->v;
			(*size)++;
			dfs(G, visited, crt->v, allVisited, actorsInProduction, size);
		}
		crt = crt->next;
	}
}

//functia gaseste componenta conexa din care face parte un nod, returnand numarul
//de noduri din componenta respectiva
long findComponent(TGraphL* G, int s, int *allVisited, int *actorsInProduction){
	int* visited = (int *)calloc(G->nn, sizeof(int));
	long size = 0;
	actorsInProduction[size] = s;
	size++;
	//se foloseste parcurgerea in adancime pentru a parcurge nodurile
	dfs(G, visited, s, allVisited, actorsInProduction, &size);
	free(visited);
	return size;
}

//functia parcurge pe nivele graful si retine distantele pana la fiecare nod
void bfs(TGraphL* graph, int s, int *distance) {

	int *visited = (int *)calloc(graph->nn, sizeof(int));
	Queue *queue = createQueue();

	visited[s] = 1;
	enqueue(queue, s);

	while(!isQueueEmpty(queue)) {
		int crt = front(queue);
		visited[crt] = 1;
		dequeue(queue);

		TNode *node = graph->adl[crt];
		while (node != NULL) {
			if(visited[node->v] == 0) {
				visited[node->v] = 1;
				distance[node->v] = distance[crt] + 1;
				enqueue(queue, node->v);
			}
			node = node->next;
		}
	}
	destroyQueue(queue);
	free(visited);
}

//functia creeaza un hashtable care tine fiecare actor si numarul nodului acestuia
ListNode **makeHashtable(FILE *input, int mod, char **allActorsCorrelation, 
	int nrMovies, long *totalActors) {

	ListNode **hashtable = (ListNode **)malloc(mod*sizeof(ListNode *));
	for(int i = 0; i < mod; i++) {
		hashtable[i] = NULL;
	}

	for(int crtMovie = 0; crtMovie < nrMovies; crtMovie++) {

		char *movie = (char *)calloc(90,sizeof(char));	
		fgets(movie, 100, input);
		free(movie);

		char *Actors = (char *)calloc(10,sizeof(char));
		fgets(Actors, 10, input);
		int nrActors = atoi(Actors);
		free(Actors); 

		for(int crtActor = 0; crtActor < nrActors; crtActor++) {
			char *actorName = (char *)malloc(80*sizeof(char));
			fgets(actorName, 80, input);

			//se calculeaza hash-ul pentru actor
			int crtHash = getHash(actorName, mod);

			//se adauga la lista cu hash-ul respectiv
			if(hashtable[crtHash] == NULL) {
				ListNode *node = (ListNode *)malloc(sizeof(ListNode));
				node->actorName = actorName;
				node->nr = *totalActors;
				(*totalActors)++;
				node->next = NULL;
				node->prev = hashtable[crtHash];
				hashtable[crtHash] = node;
				allActorsCorrelation[node->nr] = actorName;
			}
			else {

				ListNode *crtNode = hashtable[crtHash];
				int ok = 1;

				while (crtNode != NULL) {
					//daca se gaseste deja numele respectiv, nu este adaugat
					if(strcmp(crtNode->actorName, actorName) == 0) {
						ok = 0;
						free(actorName);
					}
					crtNode = crtNode->next;
				}
				if(ok == 1) {
					ListNode *node = (ListNode *)malloc(sizeof(ListNode));
					node->actorName = actorName;
					node->nr = *totalActors;
					(*totalActors)++;
					node->next = hashtable[crtHash];
					node->prev = NULL;
					hashtable[crtHash] = node;
					allActorsCorrelation[node->nr] = actorName;
				}
			}
		}
	}

	return hashtable;
}

//functia creeaza graful (cu lista de adiacenta)
TGraphL *makeGraph(FILE *input, int nrMovies, ListNode **hashtable, int mod, 
	long totalActors) {

	//se initializeaza graful
	TGraphL *graph = createGraphAdjList(totalActors);

	//se reciteste fisierul
	rewind(input);

	char *Movies = (char *)calloc(10,sizeof(char));
	fgets(Movies, 10, input);
	free(Movies);

	for( int crtMovie = 0; crtMovie < nrMovies; crtMovie++) {

		char *movie1 = (char *)calloc(90,sizeof(char));
		fgets(movie1, 100, input);
		free(movie1);

		char *Actors = (char *)calloc(10,sizeof(char));
		fgets(Actors, 10, input);
		int nrActors = atoi(Actors);
		free(Actors);

		//se creeaza o matrice cu numele actorilor filmului curent
		char **actorsInMovie = ( char **)calloc(1000,sizeof(char *));
		for(int crtActor = 0; crtActor < nrActors; crtActor++) {
			actorsInMovie[crtActor] = (char *)malloc(70*sizeof(char));
			fgets(actorsInMovie[crtActor], 70, input);
		}
		//se parcurge matricea si se adauga o muchie intre fiecare actor
		for(int x = 0; x < nrActors - 1; x++) {
			for(int y = x+1; y < nrActors; y++) {
				int hash1 = getHash(actorsInMovie[x],mod);
				int hash2 = getHash(actorsInMovie[y], mod);

				int first = getNod(hashtable, hash1, actorsInMovie[x]);
				int second = getNod(hashtable, hash2, actorsInMovie[y]);

				addEdgeList(graph, first, second);
			}
		}

		for(int crtActor = 0; crtActor < nrActors; crtActor++) {
			free(actorsInMovie[crtActor]);
		}
		free(actorsInMovie);
	}
	return graph;
}

//functia returneaza un vector care contine nodurile din productia cea mai mare
int *solveC1(int *maxnr, TGraphL *graph, long totalActors) {

	//iau un vector de vizitati pentru toate nodurile, ca sa aflu componentele
	//conexe
	int *allVisited = (int *)calloc(totalActors, sizeof(int));
	int *maxActorsName = (int *)calloc(5000, sizeof(int));
	long nrInProduction = 0;

	//parcurg fiecare nod pana gasesc un nod nevizitat
	for(int crtNode = 0; crtNode < totalActors; crtNode++) {
		if(allVisited[crtNode] == 0) {
			allVisited[crtNode] = 1;
			int *actorsInProduction = (int *)calloc(5000,sizeof(int));
			//gasesc componenta conexa de care apartine nodul respectiv
			nrInProduction = findComponent(graph, crtNode, allVisited, 
				actorsInProduction);
			//gasesc componenta conexa cun nr maxim de noduri
			if(nrInProduction > *maxnr) {
				*maxnr = nrInProduction;
				if( maxActorsName != NULL) {
					free(maxActorsName);
				}
				maxActorsName = actorsInProduction;
			}
			else {
				//dezaloc memoria daca componenta conexa nu e maxima
				free(actorsInProduction);
			}
			nrInProduction = 0;
		}
	}
	free(allVisited);

	return maxActorsName;
}

//functia scrie in fisier numele actorilor din productia cea mai mare, sortand
//in ordine alfabetica
void outputC1(FILE *output, int maxnr, char **allActorsCorrelation,
	int *maxActorsName) {

	fprintf(output,"%d\n", maxnr);

	for(int i = 0; i < maxnr-1; i++) {
		for(int j = i; j < maxnr; j++) {
			if(strcmp(allActorsCorrelation[maxActorsName[i]],
					allActorsCorrelation[maxActorsName[j]]) > 0) {
				int aux = maxActorsName[i];
				maxActorsName[i] = maxActorsName[j];
				maxActorsName[j] = aux;
			}
		}
	}

	//se foloseste vectorul care leaga numele actorilor de numarul nodului pentru
	//a scrie in fisier
	for(int i = 0; i < maxnr; i++) {
		fprintf(output,"%s", allActorsCorrelation[maxActorsName[i]]);
	}
	fclose(output);
}

//functia calculeaza distanta de la un nod de start la toate nodurile si
//returneaza distanta pana la nodul dorit
int solveC2(FILE *input,int mod, ListNode **hashtable, TGraphL *graph) {

	char *firstActor = (char *)calloc(100,sizeof(char));
	fgets(firstActor, 100, input);
	char *secondActor = (char *)calloc(100,sizeof(char));
	fgets(secondActor, 100, input);

	int firstHash = getHash(firstActor, mod);
	int secondHash = getHash(secondActor, mod);

	//se gaseste nodul aferent fiecarui actor
	int firstNode = getNod(hashtable, firstHash, firstActor);
	int secondNode = getNod(hashtable, secondHash, secondActor);

	//se parcurge graful pe nivele, modficandu-se in vector distanta
	int *distance = (int *)calloc(10000,sizeof(int));
	bfs(graph, firstNode, distance);

	free(firstActor);
	free(secondActor);

	int dist = distance[secondNode];
	free(distance);

	return dist;
}

//functia sterge hashtable-ul
void destroyHashtable(ListNode **hashtable, int mod) {

	ListNode *crt;
	//se elibereaza memoria pentru fiecare lista din hashtable in parte
	for(int i = 0; i < mod; i++) {
		crt = hashtable[i];
		while(crt != NULL) {
			ListNode* aux = crt;
			crt = crt->next;
			free(aux);
		}
	}
	free(hashtable);
}

//functia returneaza minimul dintre doua valori
int minval( int a, int b) {
	if( a > b) {
		return b;
	}
	return a;
}

//functia parcurge nodurile si, la momentul gasirii unei punti, o adauga in
//vector si incrementeaza numarul total
void dfsB(TGraphL *graph, int v, int time, int *idx, int *low, int *pi,
 Pair *puntiVec, int *nrPunti, char **allActorsCorrelation) {

	idx[v] = time;
	low[v] = time;
	time++;

	TNode *crt = graph->adl[v];
	while( crt != NULL ) {
		if( crt->v != pi[v]) {
			if (idx[crt->v] == -1) {
				pi[crt->v] = v; 

				dfsB(graph, crt->v, time, idx, low, pi, puntiVec, nrPunti, 
					allActorsCorrelation);
				low[v] = minval(low[crt->v], low[v]);
				if (low[crt->v] > idx[v] ) {
					//cei doi actori care formeaza o punte se adauga ordonat
					//alfabetic in structura de punte
					if(strcmp(allActorsCorrelation[v], 
							allActorsCorrelation[crt->v]) < 0 ) {
						puntiVec[*nrPunti].actor1 = v;
						puntiVec[*nrPunti].actor2 = crt->v;
					}
					else {
						puntiVec[*nrPunti].actor1 = crt->v;
						puntiVec[*nrPunti].actor2 = v;
					}
					(*nrPunti)++;

				} 
			}
			else {
				low[v] = minval(low[v], idx[crt->v]);
			}
		}
		crt = crt->next; 
	}

}

//functia returneza un vector cu puntile gasite in graf
Pair *punti(TGraphL *graph, long totalActors, int *nrPunti,
	char **allActorsCorrelation) {

 	int time = 0;
 	int *idx = (int *)malloc(totalActors*sizeof(int));
 	int *low = (int *)malloc(totalActors*sizeof(int));
 	int *pi = (int *)malloc(totalActors*sizeof(int));

 	Pair *puntiVec = (Pair *)malloc(1000*sizeof(Pair));

	for(int v = 0; v < totalActors; v++) {
		idx[v] = -1;
		low[v] = INT_MAX;
		pi[v] = -1;
	}
	for(int v = 0; v < totalActors; v++) {
		if(idx[v] == -1) {
			//se foloseste de o functie auxiliara care gaseste puntile
			dfsB(graph, v, time, idx, low, pi, puntiVec, nrPunti, 
				allActorsCorrelation);
		}
	}
	free(idx);
	free(low);
	free(pi);
	return puntiVec;
}

//functia sorteaza vectorul de punti
void sortPairs( Pair *solutie, int nrPunti, char **allActorsCorrelation) {

	for(int i = 0; i < nrPunti - 1; i++) {
		for(int j = i; j < nrPunti ; j++) {
			//interschimbam doua punti daca primul actor din prima punte este mai
			//mare lexicografic decat primul actor din a doua punte
			if(strcmp(allActorsCorrelation[solutie[i].actor1], 
				allActorsCorrelation[solutie[j].actor1]) > 0) {
				Pair aux = solutie[i];
				solutie[i] = solutie[j];
				solutie[j] = aux;
			}
			else if( strcmp(allActorsCorrelation[solutie[i].actor1],
						allActorsCorrelation[solutie[j].actor1]) == 0) {
				//interschimbam doua punti daca primii actori sunt aceiasi, dar 
				//al doilea actor din prima punte este mai mare lexicografic 
				//decat al doilea
				if( strcmp(allActorsCorrelation[solutie[i].actor2],
						allActorsCorrelation[solutie[j].actor2]) > 0) {
					Pair aux = solutie[i];
					solutie[i] = solutie[j];
					solutie[j] = aux;
				}
			}
		}
	}
}

//functia scrie in fisier puntile sortate
void outputC3(FILE *output, char **allActorsCorrelation, int nrPunti,
	Pair *solutie) {

		fprintf(output, "%d\n", nrPunti);
		for(int i = 0; i < nrPunti; i++) {

			int len1 = strlen(allActorsCorrelation[solutie[i].actor1]);
			int len2 = strlen(allActorsCorrelation[solutie[i].actor2]);
			char *first = (char *)calloc(len1, sizeof(char));
			char *second = (char *)calloc(len2, sizeof(char));

			//elimin caracterul "\n" de la sfarsiul randului
			strncpy(first, allActorsCorrelation[solutie[i].actor1], len1-1);
			strncpy(second, allActorsCorrelation[solutie[i].actor2], len2-1);
			fprintf(output, "%s %s\n", first, second);
			free(first);
			free(second);
		}
		fclose(output);
}

int main(int argc, char *argv[]) {

	//aleg un nr prim indeajuns de mare pentru a nu se forma liste lungi
	int mod = 700067;

	//deschid fisierele de input si output si retin comanda ce se vrea realizata
	char *command = argv[1];
	FILE *input = fopen(argv[2], "r");
	FILE *output = fopen(argv[3], "w");

	//citesc linie cu linie
	char *Movies = (char *)calloc(10,sizeof(char));
	fgets(Movies, 10, input);

	//trasnform sirurile care contin un numar in intregi
	int nrMovies = atoi(Movies);
	free(Movies);

	long totalActors = 0;

	//creez o matrice in care retin numele actorului la pozitia corespunzatoare
	//nodului
	char **allActorsCorrelation = (char **)malloc(7000*sizeof(char *));

	ListNode **hashtable = makeHashtable(input, mod, allActorsCorrelation,
	 nrMovies, &totalActors);

	TGraphL *graph = makeGraph(input, nrMovies, hashtable, mod, totalActors);

	//realizez instructiunile pentru indeplinirea primului task
	if( strcmp(command, "-c1") == 0 ) {

		int maxnr = -1;
		int *maxActorsName = solveC1(&maxnr, graph, totalActors);
		outputC1(output, maxnr, allActorsCorrelation, maxActorsName);
		free(maxActorsName);
	}


	if( strcmp(command, "-c2") == 0) {
		
		//aflu distanta minima
		int minDistance = solveC2(input, mod, hashtable, graph);
		//daca distanta e 0, inseamna ca nodurile nu sunt conectte, deci returnez
		//-1
		if(minDistance == 0) {
			fprintf(output,"-1\n");
		}
		else {
			fprintf(output, "%d\n", minDistance);
		}
		fclose(output);
	}

	if( strcmp(command, "-c3") == 0) {
		int nrPunti = 0;

		//obtin vectorul de punti
		Pair *solutie = punti(graph, totalActors,&nrPunti,allActorsCorrelation);

		//sortez vectorul si scriu in fisierul de output
		sortPairs(solutie, nrPunti, allActorsCorrelation);
		outputC3(output, allActorsCorrelation, nrPunti, solutie);
		free(solutie);
	}

	//eliberez memoria alocata grafului si hashtable
	destroyGraphAdjList(graph);
	destroyHashtable(hashtable, mod);

	for(int j = 0; j < totalActors; j++) {
		free(allActorsCorrelation[j]);
	}
	free(allActorsCorrelation);
	fclose(input);

	return 0;
}