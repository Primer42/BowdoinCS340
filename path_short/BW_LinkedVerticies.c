
#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "BW_DArray.h"
#include "W_GatherTigerData.h"

#include "BW_LinkedVerticies.h"

#define VERTICIES_INCREMENT 100000
#define DUMMY_ANGLE 400000000

Node* DUMMY_NODE;
int VERTEXCOMPARE_DEBUG = 0;

#define LINKED_VERTICIES_DEBUG if(1)

//construct LinkVerticies from preconstructed data
DArray* createLinkedVerticies(TigerData* data) {
  assert(data);

  //setup dummy node
  DUMMY_NODE = (Node*) malloc(sizeof(Node)); assert(DUMMY_NODE);
  DUMMY_NODE->longitude = DUMMY_ANGLE;
  DUMMY_NODE->latitude = DUMMY_ANGLE;
  
  //create verticies (dynamic array of void*'s)
  DArray* verticies = (DArray*) malloc(sizeof(DArray)); assert(verticies);
  verticies = newDArray(VERTICIES_INCREMENT, VERTICIES_INCREMENT);

  //fills start points into verticies
  long numStartNodes = fillStartPoints(verticies, data);
  // printVerticies(verticies);

  //fills end points into verticies
  fillEndPoints(verticies, data, numStartNodes);

  //  VERTEXCOMPARE_DEBUG = 1;
  //link the nodes
  linkPoints(verticies, data);
  //  VERTEXCOMPARE_DEBUG = 0;
  
  LINKED_VERTICIES_DEBUG{printVerticies(verticies);}
  return verticies;
}

//fills start points (RT1)
long fillStartPoints(DArray* verticies, TigerData* data) {
  assert(verticies); assert(data);
  LINKED_VERTICIES_DEBUG{printf("START FILL START POINTS\n"); fflush(stdout);}
  long numStartNodes = 0;
  long numNodesInCompletedCounites = 0;
  
  int stateCount;
  for(stateCount = 0; stateCount < data->numStates; stateCount++) {
    int countyCount;

    for(countyCount = 0;
	countyCount < data->states[stateCount].numCounties;
	countyCount++) {

      //sort county by start point
      qsort(data->states[stateCount].counties[countyCount].chains,
	    data->states[stateCount].counties[countyCount].numChains,
	    sizeof(Chain*), compareChainsStart);

      //veriable to keep track of what type of "node" we're looking for
      Node* previous = DUMMY_NODE;

      long chainCount;
      for(chainCount = 0;
	  chainCount < data->states[stateCount].counties[countyCount].numChains;
	  chainCount++) {

	if(data->states[stateCount].counties[countyCount].chains[chainCount]->CFCC[0] == 'A'){
	
	  //working node
	  Node* currentNode =
	    &data->states[stateCount].counties[countyCount].chains[chainCount]->start;
	
	  //if the node is not the same as before (previous)
	  if((currentNode->longitude != previous->longitude) ||
	     (currentNode->latitude != previous->latitude)) {
	    
	    //make and search for new Vertex in verticies
	    Vertex* currentVertex =
	      (Vertex*) malloc(sizeof(Vertex)); assert(currentVertex);
	    currentVertex->start = currentNode;
	    //printf("checking (%d, %d)", currentVertex->start->longitude, currentVertex->start->latitude);
	    Vertex* existingVertex;
	    //if bsearch tries to search 0 elements, it causes a seg fault, so we don't search if there are 0 or fewer elements to search
	    if(numNodesInCompletedCounites > 0){
	      existingVertex =
		*(Vertex**) bsearch(&currentVertex, &(verticies->content[0]),
				    numNodesInCompletedCounites, 
				    sizeof(Vertex*),
				    (void*)VertexCompare);
	    }
	    else{
	      existingVertex = NULL;
	    }
	    //completely new Vertex needs to be added
	    if(existingVertex == NULL) {
	      addVertex(verticies, currentNode);
	      numStartNodes++;
	      //printf("got here: chain# %ld\n", numStartNodes); fflush(stdout);
	    }

	    free(currentVertex);
	    
	    previous = currentNode;  
	  }
	}
      }
      numNodesInCompletedCounites = verticies->size;
      //printf("num nodes in completed counties = %ld\n", numNodesInCompletedCounites); fflush(stdout);
      qsort(&(verticies->content[0]), verticies->size, 
	    sizeof(Vertex*), (void*)VertexCompare);
    }
  }
  LINKED_VERTICIES_DEBUG{  printf("END FILL START POINTS\n");fflush(stdout);}
  return numStartNodes;
}

//fill end points (RT1)
void fillEndPoints(DArray* verticies, TigerData* data, long numStartNodes) {
  assert(verticies); assert(data);
  LINKED_VERTICIES_DEBUG{  printf("START FILL END POINTS\n"); fflush(stdout);}

  long numVerticiesAddedSoFar = verticies->size;

  qsort(&(verticies->content[0]), verticies->size, sizeof(Vertex*), 
	(void*)VertexCompare);

  int stateCount;
  for(stateCount = 0; stateCount < data->numStates; stateCount++) {
    int countyCount;
    for(countyCount = 0;
	countyCount < data->states[stateCount].numCounties;
	countyCount++) {

      //sort by end points
      qsort(&(data->states[stateCount].counties[countyCount].chains[0]),
	    data->states[stateCount].counties[countyCount].numChains,
	    sizeof(Chain*), compareChainsEnd);


      //working variable to track what kind of "Node" that being looked at
      Node* previous = DUMMY_NODE;

      long chainCount;
      for(chainCount = 0;
	  chainCount < data->states[stateCount].counties[countyCount].numChains;
	  chainCount++) {

	if(data->states[stateCount].counties[countyCount].chains[chainCount]->CFCC[0] == 'A'){
	  Node* currentNode =
	    &data->states[stateCount].counties[countyCount].chains[chainCount]->end;

	  //if got unique next node (not same as previous)
	  if((currentNode->longitude != previous->longitude) ||
	     (currentNode->latitude != previous->latitude)) {
	    
	    //update previous
	    previous = currentNode;
	    
	    //make and search for new Vertex in verticies
	    Vertex* currentVertex =
	      (Vertex*) malloc(sizeof(Vertex)); assert(currentVertex);
	    currentVertex->start = currentNode;
	    Vertex* existingVertex =
	      (Vertex*) bsearch(&currentVertex, &(verticies->content[0]),
				numVerticiesAddedSoFar, sizeof(Vertex*),
				(void*)VertexCompare);	
	    //completely new Vertex needs to be added
	    if(existingVertex == NULL) {
	      addVertex(verticies, currentVertex->start);
	    }
	    free(currentVertex);
	  }
	}
      }
      numVerticiesAddedSoFar = verticies->size;  
      qsort(&(verticies->content[0]), verticies->size, 
	    sizeof(Vertex*), (void*)VertexCompare);
    }
  }
  LINKED_VERTICIES_DEBUG{  printf("END FILL END POINTS\n"); fflush(stdout);}
}

//creates a Vertex and add into verticies
void addVertex(DArray* verticies, Node* start) {
  assert(verticies); assert(start);

  Vertex* newVertex = (Vertex*) malloc(sizeof(Vertex)); assert(newVertex);
  newVertex->start = start;
  newVertex->next =
    (Connection*) malloc(sizeof(Connection)); assert(newVertex->next);
  Vertex* dummyVertex = (Vertex*) malloc(sizeof(Vertex)); assert(dummyVertex);
  dummyVertex->start = DUMMY_NODE;
  newVertex->next->end = dummyVertex;

  DArrayAdd(verticies, newVertex);
}

// linking verticies
void linkPoints(DArray* verticies, TigerData* data) {
  assert(verticies); assert(data);

  //sort verticies
  qsort(&(verticies->content[0]), verticies->size, sizeof(Vertex*),
	(void*)VertexCompare);

  printVerticies(verticies);

  int stateCount;
  for(stateCount = 0; stateCount < data->numStates; stateCount++) {
    int countyCount;
    for(countyCount = 0;
	countyCount < data->states[stateCount].numCounties;
	countyCount++) {
      long chainCount;
      for(chainCount = 0;
	  chainCount < data->states[stateCount].counties[countyCount].numChains;
	  chainCount++) {

	if(data->states[stateCount].counties[countyCount].chains[chainCount]->CFCC[0] == 'A'){
	  
	  //make Vertex from RT1 chain for searching for both start and end nodes

	  Vertex* tempVertex = (Vertex*) malloc(sizeof(Vertex));
	  assert(tempVertex);
	  tempVertex->start =
	    &data->states[stateCount].counties[countyCount].chains[chainCount]->start;	  
	  Vertex* startVertex = *(Vertex**) bsearch(&tempVertex, 
						  &(verticies->content[0]), 
						  verticies->size, 
						  sizeof(Vertex*),
						  (void*)VertexCompare);
	  
	  tempVertex->start =
	    &data->states[stateCount].counties[countyCount].chains[chainCount]->end;
	  Vertex* endVertex = *(Vertex**) bsearch(&tempVertex, 
						&(verticies->content[0]), 
 						verticies->size,
						sizeof(Vertex*),
						(void*)VertexCompare);
	  
	  //link start and end
	  VertexLink(startVertex, endVertex);
	  //printVerticies(verticies);

	  
	  free(tempVertex);

	}
      }
    }
  }
}  

//linking verticies
void VertexLink(Vertex* v1, Vertex* v2) {
  assert(v1); assert(v2);
  printf("starting to link (%d, %d) and (%d, %d)\n", v1->start->longitude, v1->start->latitude, v2->start->longitude, v2->start->latitude); fflush(stdout);


  float distance = sqrt(pow(v1->start->longitude-v2->start->longitude, 2) +
			pow(v1->start->latitude-v2->start->latitude, 2));

  Connection* c2 = (Connection*) malloc(sizeof(Connection)); assert(c2);
  c2->end = v1;
  c2->distance = distance;
  c2->next = v2->next;
  v2->next = c2;

  Connection* c1 = (Connection*) malloc(sizeof(Connection)); assert(c1);
  c1->end = v2;
  c1->distance = distance;
  c1->next = v1->next;
  v1->next = c1;

}
			

//#define VERTEXCOMPARE_DEBUG if(0)

//compare Vertexs- needs Vertex**
//start by comparing longitude- if longitudes ==, compare latitudes
int VertexCompare(void* void1, void* void2) {
  assert(void1); assert(void2);
  
  Vertex* vertex1 = *(Vertex**)void1;
  Vertex* vertex2 = *(Vertex**)void2;

  //printf("comparing vertexes (%d, %d) and (%d, %d)\n", vertex1->start->longitude, vertex1->start->latitude, vertex2->start->longitude, vertex2->start->latitude); fflush(stdout);

  if(VERTEXCOMPARE_DEBUG){printf("vertex1=(%d, %d) vertex2=(%d, %d)\t", vertex1->start->longitude, vertex1->start->latitude, vertex2->start->longitude, vertex2->start->latitude); fflush(stdout);}
  
  if(vertex1->start->longitude > vertex2->start->longitude) {
    if(VERTEXCOMPARE_DEBUG){printf("1>2\n"); fflush(stdout);}
    return 1;
  } else if(vertex1->start->longitude == vertex2->start->longitude) {
    if(vertex1->start->latitude > vertex2->start->latitude) {
      if(VERTEXCOMPARE_DEBUG){printf("1>2\n"); fflush(stdout);}
      return 1;
    } else if(vertex1->start->latitude == vertex2->start->latitude) {
      if(VERTEXCOMPARE_DEBUG){printf("1=2\n"); fflush(stdout);}
      return 0;
    }else{
      if(VERTEXCOMPARE_DEBUG){printf("1<2\n"); fflush(stdout);}
      return -1;
    }
  } else{
    if(VERTEXCOMPARE_DEBUG){printf("1<2\n"); fflush(stdout);}
    return -1;
  }
}

//helper method to compare Vertex*, rather than Vertex**
int VertexComparePointers(Vertex* v1, Vertex* v2){
  assert(v1); assert(v2);
  return VertexCompare(&v1, &v2);
}


//printf function for verticies
void printVerticies(DArray* verticies) {

  printf("VERTICIES SIZE: %ld\n", verticies->size);

  int i;
  for(i = 0; i < verticies->size; i++) {
    Vertex* v1 = (Vertex*) verticies->content[i];
    
    printf("Vertex:(%d, %d)\n", v1->start->longitude, v1->start->latitude);
    Connection* con = v1->next;
    printf("address = %d\n", (int)v1); fflush(stdout);
    printf("Connections:\n");
    while(con->end->start->longitude != DUMMY_ANGLE) {
	  printf("(%d, %d)\t",
		 con->end->start->longitude, con->end->start->latitude);
	  printf("dist: %f\n", con->distance);
	  con = con->next;
    }
    printf("\n");
  }
  fflush(stdout);
}
