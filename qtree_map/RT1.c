#include "RT1.h"

/*-CONSTRUCT & KILL-----------------------------------------------------------*/
// creates and returns a new RT1; assume that inType is a valid string
RT1* RT1_new(char* inType, 
	     int inStartLon, int inStartLat, int inEndLon, int inEndLat) {
  assert(inType);

  RT1* newRT1 = (RT1*) malloc(sizeof(RT1)); assert(newRT1);
  //newRT1->type = (char*) malloc(sizeof(char) * (strlen(inType)+1));
  strcpy(newRT1->type, inType);
  Node_fill(&newRT1->start, inStartLon, inStartLat);
  Node_fill(&newRT1->end, inEndLon, inEndLat);
  //newRT1->dist = RT1_calcDist(*newRT1);
  return newRT1;
}

// fill an RT1*
void RT1_fill(RT1* rt1, char* inType,
	      int inStartLon, int inStartLat, int inEndLon, int inEndLat) {
  assert(rt1); 
  //printf("RT1_fill: at rt1=%d, inType=%s", rt1, inType);  fflush(stdout);
  strncpy(rt1->type, inType, 4);
  //printf("copied type\n"); fflush(stdout);
  Node_fill(&rt1->start, inStartLon, inStartLat);
  Node_fill(&rt1->end, inEndLon, inEndLat);
  //  rt1->dist = RT1_calcDist(*rt1);
  //printf("RT1_fill: done\n");  fflush(stdout);
}

/*-GETTERS--------------------------------------------------------------------*/
// returns the type of <rt1>
char* RT1_getType(RT1* rt1) {
  assert(rt1);
  return rt1->type;
}

// returns the start node of <rt1>
Node RT1_getStart(RT1 rt1) {
  return rt1.start;
}

// returns the end node of <rt1>
Node RT1_getEnd(RT1 rt1) {
  return rt1.end;
}

// retusn the length of <rt1>
/* float RT1_getDist(RT1 rt1) { */
/*   return rt1.dist; */
/* } */

/*-UTILITY--------------------------------------------------------------------*/
// calculates and return the distance between the start and end of <rt1>
float RT1_calcDist(RT1 rt1) {
  return sqrt(pow(Node_getLon(rt1.start)-Node_getLon(rt1.end),2) +
	      pow(Node_getLat(rt1.start)-Node_getLat(rt1.end),2));
}
