#ifndef __Node_h
#define __Node_h

#include <assert.h>
#include <stdlib.h>

typedef struct node_t {
  int lon;
  int lat;
} Node;

// creates and returns a new node
Node* Node_new(int inLon, int inLat);

//sets the nodes passed in with the lat/lon values passed in
void Node_fill(Node* node, int inLon, int inLat);

// kills <node>
void Node_kill(Node* node);

// returns the lon of <node>
int Node_getLon(Node node);

// returns the lat of <node>
int Node_getLat(Node node);

#endif
