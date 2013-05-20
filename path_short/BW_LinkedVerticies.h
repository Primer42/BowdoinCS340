#ifndef __BW_LinkedVerticies_h
#define __BW_LinkedVerticies_h

#include "W_GatherTigerData.h"
#include "BW_DArray.h"

typedef struct _vertex {
  Node* start;
  struct _connection* next;
} Vertex;

typedef struct _connection {
  Vertex* end;
  float distance;
  struct _connection* next;
} Connection;

//construct LinkedVerticies
DArray* createLinkedVerticies(TigerData* data);

//fills start points (RT1)
long fillStartPoints(DArray* verticies, TigerData* data);

//fill end points (RT1)
void fillEndPoints(DArray* verticies, TigerData* data, long numStartNodes);

//creates a Vertex and add into verticies
void addVertex(DArray* verticies, Node* start);

//link end points (RT1)
void linkPoints(DArray* verticies, TigerData* data);

//linking verticies
void VertexLink(Vertex* v1, Vertex* v2);

//compare Vertexs- needs Vertex**
int VertexCompare(void* v1, void* v2);

//compare vertex*
int VertexComparePointers(Vertex* v1, Vertex* v2);

//printf fuction for verticies
void printVerticies(DArray* verticies);

#endif
