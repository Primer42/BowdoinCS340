#ifndef __memTile_h
#define __memTile_h

#include <assert.h>

#include "RT1.h"

#define DYNAMIC_INCREMENT 10000

typedef struct memTile_t {
  // dynamic RT1 array that stores data in current tile
  RT1* data;
  int dataSize;
  int dataMax;
  int dataInc;
  // stores boundries
  int minLon, minLat, maxLon, maxLat;
} MemTile;

/*-CONSTRUCT & DESTROY--------------------------------------------------------*/
// initializes a pre-malloced MemTile <tile>
void MemTile_fill(MemTile* tile,
		  int inMinLon, int inMinLat, int inMaxLon, int inMaxLat);

// add a RT1 into MemTile
void MemTile_addRT1(MemTile* tile, char* type,
		    int sLon, int sLat, int eLon, int eLat);

/*-HELPER---------------------------------------------------------------------*/
// grows <data> of <tile> by <dataInc>
void MemTile_growArray(MemTile* tile);

/*-GETTERS--------------------------------------------------------------------*/
// returns total number of data in <tile>
int MemTile_dataSize(MemTile tile);

// returns RT1* of <tile>
RT1* MemTile_getRT1(MemTile tile);

// returns minLon
int MemTile_minLon(MemTile tile);

// returns minLat
int MemTile_minLat(MemTile tile);

// returns maxLon
int MemTile_maxLon(MemTile tile);

// returns maxLat
int MemTile_maxLat(MemTile tile);

#endif
