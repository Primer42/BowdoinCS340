#ifndef __RT1_h
#define __RT1_h

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "Node.h"

typedef struct rt1_t {
  char type[4];
  Node start;
  Node end;
  //float dist;
} RT1;


/*-CONSTRUCT & KILL-----------------------------------------------------------*/
// creates and returns a new RT1
RT1* RT1_new(char* inType,
	     int inStartLon, int inStartLat, int inEndLon, int inEndLat);

// fill an RT1*
void RT1_fill(RT1* rt1, char* inType,
	      int inStartLon, int inStartLat, int inEndLon, int inEndLat);

/*-GETTERS--------------------------------------------------------------------*/
// returns the type of <rt1>
char* RT1_getType(RT1* rt1);

// returns the start node of <rt1>
Node RT1_getStart(RT1 rt1);

// returns the end node of <rt1>
Node RT1_getEnd(RT1 rt1);

// retusn the length of <rt1>
//float RT1_getDist(RT1 rt1);

/*-UTILITY--------------------------------------------------------------------*/
// calculates and return the distance between the start and end of <rt1>
float RT1_calcDist(RT1 rt1);

#endif
