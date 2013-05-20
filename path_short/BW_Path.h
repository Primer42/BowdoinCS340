#ifndef _BW_Path_h
#define _BW_Path_h

#include "W_GatherTigerData.h"
#include "BW_LinkedVerticies.h"
#include "BW_DArray.h"

// path strcutre to store a path: to be used in priorit queue
typedef struct Path_t{
  // array of pointers to nodes corrosponding to this path
  DArray* verticies;
  // total distance in this path
  float dist;
} Path;

// creates and returns a new Path object with <start> as 1st Vertex
Path* newPath(Vertex* start);

// creates and returns an identical copy of <path>
Path* pathClone(Path* path);

// adds a connection to the path
void pathAddVertex(Path* path, Vertex* vertex, float extraDist);

// returns second to last Vertex*
Vertex* getAlmostEnd(Path* path);

// returns the last Vertex*
Vertex* getEnd(Path* path);

// returns dist
float pathGetDist(Path* path);

// returns priority (dist)
float getPriority(Path path);

//print the passed path
void printPath(Path path);

#endif
