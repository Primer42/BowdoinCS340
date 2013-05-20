#include "MemTile.h"

/*-CONSTRUCT & DESTROY--------------------------------------------------------*/
// initializes a pre-malloced MemTile <tile>
void MemTile_fill(MemTile* tile,
		  int inMinLon, int inMinLat, int inMaxLon, int inMaxLat) {
  assert(tile);

  tile->dataSize = 0;
  tile->dataMax = DYNAMIC_INCREMENT;
  tile->dataInc = DYNAMIC_INCREMENT;

  tile->minLon = inMinLon;
  tile->minLat = inMinLat;
  tile->maxLon = inMaxLon;
  tile->maxLat = inMaxLat;

  tile->data = (RT1*) malloc(tile->dataInc*sizeof(RT1)); assert(tile->data);
  //int i;
  //for(i = 0; i < tile->dataInc; i++){
  //tile->data[i].type = (char*) malloc(4 * sizeof(char));
  //}
}

// add a RT1 into MemTile
void MemTile_addRT1(MemTile* tile, char* type,
		    int sLon, int sLat, int eLon, int eLat) {
  assert(tile); assert(type);
  //printf("MemTile_addRT1: start"); fflush(stdout);
  assert(tile->data); 
  if(tile->dataSize + 1== tile->dataMax)
    MemTile_growArray(tile);

  RT1_fill(&(tile->data[tile->dataSize]), type, sLon, sLat, eLon, eLat);
  tile->dataSize++;
  //printf("MemTile_addRT1: end\n"); fflush(stdout);
}

/*-HELPER---------------------------------------------------------------------*/
// grows <data> of <tile> by <dataInc>
void MemTile_growArray(MemTile* tile) {
  assert(tile);

  //printf("mem_tle: grow: start");  fflush(stdout); 
  tile->data = (RT1*) realloc(tile->data,
			      (tile->dataMax + tile->dataInc) * sizeof(RT1));
  assert(tile->data);
  tile->dataMax = tile->dataMax + tile->dataInc;
  //printf("mem_tle: grow: end");  fflush(stdout); 
}

/*-GETTERS--------------------------------------------------------------------*/
// returns total number of data in <tile>
int MemTile_dataSize(MemTile tile) {
  return tile.dataSize;
}

// returns RT1* of <tile>
RT1* MemTile_getRT1(MemTile tile) {
  return tile.data;
}

// returns minLon
int MemTile_minLon(MemTile tile) {
  return tile.minLon;
}

// returns minLat
int MemTile_minLat(MemTile tile) {
  return tile.minLat;
}

// returns maxLon
int MemTile_maxLon(MemTile tile) {
  return tile.maxLon;
}

// returns maxLat
int MemTile_maxLat(MemTile tile) {
  return tile.maxLat;
}
