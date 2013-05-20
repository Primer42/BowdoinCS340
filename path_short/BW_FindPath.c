#include <assert.h>

#include "BW_FindPath.h" 

// finds and return a Path structure from <start> to <end> 
// with <dummy> ans a dummy node
Path* findShortPath(Vertex* start, Vertex* end, Vertex* dummy) {
  assert(start); assert(end); assert(dummy);

  PQueue* queue = PQ_initialize();
  
  //get the Connections for the start node so we can add the first set of verticies
  Connection* tracker = start->next;

  while(VertexComparePointers(tracker->end, dummy) != 0) {
    
    Path* startPath = newPath(start);
    pathAddVertex(startPath, tracker->end, tracker->distance);
    
    PQ_insert(queue, *startPath);

    free(startPath);

    tracker = tracker->next;
  }

  //after first round of additions, search for the rest of the path
  Path* solution = SearchPath(end, queue, dummy);

  //now that we have the solution, we don't need the PQ anymore
  PQ_delete(queue);
  return solution;
}

// finds and return shortest path startin from <queue> to the <end>
Path* SearchPath(Vertex* end, PQueue* queue, Vertex* dummy) {
  assert(end); assert(queue); assert(dummy);

  //malloc a path to hold the one we are looking at
  Path* curPath = (Path*) malloc(sizeof(Path)); assert(curPath);
  
  //make sure the queue isn't empty to begin with
  if(!PQ_isEmpty(queue)) {

    //get the first lowest value
    PQ_extractMin(queue, curPath);

    //if we haven't made it to the end, we need to keep searching
    while(VertexComparePointers(getEnd(curPath), end) != 0) {

      //get the cur path's end point's connections, so they can be added
      Connection* tracker = getEnd(curPath)->next;
      
      //add all the valid new points i.e. not dummy and not going backwards
      while(VertexComparePointers(tracker->end, dummy) != 0){
	if(VertexComparePointers(tracker->end, getAlmostEnd(curPath)) != 0) {	

	  //make a new path and add the new vertex to it
	  Path* newPath = pathClone(curPath);
	  pathAddVertex(newPath, tracker->end, tracker->distance);

	  printf("test 1 (%d, %d)\n", getEnd(newPath)->start->longitude, getEnd(newPath)->start->latitude); fflush(stdout);
	  //add the new path to the PQ
	  PQ_insert(queue,*newPath);
	  printf("test 2 (%d, %d)\n", getEnd(newPath)->start->longitude, getEnd(newPath)->start->latitude); fflush(stdout);
;

	}

	//get the next new vertex and go back
	tracker = tracker->next;
      }

      printf("\n"); PQ_print(queue); fflush(stdout);
      
      //all possible paths have been exausted, so return NULL
      if(PQ_isEmpty(queue))
	return NULL;
      //otherwise, get the next shortest path
      else
	PQ_extractMin(queue, curPath);

      printf("end of loop\n"); fflush(stdout);
    }
    
    //we have a path that finished, so return it.
    return curPath;
  }

  //the PQ was empty, so return NULL
  return NULL;
}
