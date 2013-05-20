#include "Node.h"

// creates and returns a new node
Node* Node_new(int inLon, int inLat) {
  Node* newNode = (Node*) malloc(sizeof(Node)); assert(newNode);
  newNode->lon = inLon;
  newNode->lat = inLat;
  return newNode;
}

//sets the nodes passed in with the lat/lon values passed in
void Node_fill(Node* node, int inLon, int inLat) {
  assert(node);
  node->lon = inLon;
  node->lat = inLat;
}

// kills <node>
void Node_kill(Node* node) {
  assert(node);
  free(node);
}

// returns the lon of <node>
int Node_getLon(Node node) {
  return node.lon;
}

// returns the lat of <node>
int Node_getLat(Node node) {
  return node.lat;
}
