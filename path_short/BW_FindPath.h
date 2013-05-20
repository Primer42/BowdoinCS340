#ifndef _BW_FindPath_h
#define _BW_FindPath_h

#include <stdlib.h>
#include "pqheap.h"
#include "BW_DArray.h"
#include "BW_LinkedVerticies.h"
#include "BW_Path.h"

// finds and return a Path structure from <start> to <end> 
// with <dummy> ans a dummy node
Path* findShortPath(Vertex* start, Vertex* end, Vertex* dummy);

// finds and return shortest path startin from <queue> to the <end>
Path* SearchPath(Vertex* end, PQueue* queue, Vertex* dummy);

#endif
