#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "BW_Path.h"

// creates and returns a new Path object with <start> as 1st Vertex
Path* newPath(Vertex* start) {
  assert(start);

  Path* new = (Path*) malloc(sizeof(Path)); assert(new);
  new->verticies = newDArray(10, 10);
  DArrayAdd(new->verticies, start);
  new->dist = 0;

  return new;  
}

// creates and returns an identical copy of <path>
Path* pathClone(Path* path) {
  assert(path);

  Path* new = (Path*) malloc(sizeof(path)); assert(new);
  new->verticies = DArrayClone(path->verticies);
  new->dist = path->dist;

  return new;
}

// adds a connection to the path
void pathAddVertex(Path* path, Vertex* vertex, float extraDist) {
  assert(path); assert(vertex);

  DArrayAdd(path->verticies, vertex);
  path->dist = path->dist + extraDist;
}

// returns second to last Vertex*
Vertex* getAlmostEnd(Path* path) {
  assert(path);

  return (Vertex*) path->verticies->content[path->verticies->size-2];
}

// returns the last Vertex*
Vertex* getEnd(Path* path) {
  assert(path);

  return (Vertex*) path->verticies->content[path->verticies->size-1];
}

// returns dist
float pathGetDist(Path* path) {
  assert(path);

  return path->dist;
}

// returns priority (dist)
float getPriority(Path path) {
  return path.dist;
}

//print the passed path
void printPath(Path path){
  int i;
  printf("start");
  for(i = 0; i < path.verticies->size; i++){
    Vertex* v = (Vertex*)(path.verticies->content[i]);
    printf("->(%d, %d)", v->start->longitude, v->start->latitude); 
  }
  printf("->end\tdistance = %f", path.dist);
  fflush(stdout);
}

