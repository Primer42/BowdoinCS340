#ifndef __Map_h
#define __Map_h

#include <assert.h>
#include <math.h>
#include <string.h>

#include "FileTile.h"
#include "MemTile.h"
#include "RT1.h"

#define MEMTILE_LEVEL_COUNT 5

#define MIN_LON -125000000
#define MIN_LAT 24000000

#define CELL_WIDTH 2000000
#define CELL_HEIGHT 1000000

// fileTile's location and base name
#define TILE_NAME "DATA/TILE_"

typedef struct map_t {
  //fileTiles
  FileTile** fileTiles;
  int fileTileCol;
  int fileTileRow;

  //memTiles and col/row storage
  MemTile** memTiles[MEMTILE_LEVEL_COUNT];
  int memTileCol[MEMTILE_LEVEL_COUNT];
  int memTileRow[MEMTILE_LEVEL_COUNT];
} Map;

/*-CONSTRUCT & DESTROY--------------------------------------------------------*/
// creates and return a Map
Map* Map_new(int clearFiles);

// adds an RT1 to all the fileTiles in <map> that the RT1 goes through
void Map_addRT1(Map* map, char* type, int lon1, int lat1, int lon2, int lat2);

// adds RT1 to corrosponding MemTiles
void Map_addRT1toMemTiles(Map* map, int col, int row, char* type,
			  int sLon, int sLat, int eLon, int eLat);

/*-HELPERS--------------------------------------------------------------------*/
// initializes FileTiles in <map>
void Map_initFileTiles(Map* map);

// initializes MemTiles in <map>
void Map_initMemTiles(Map* map);

// decides if rt1 of type <type> needs to go into level <level>
int Map_typeInLevel(char* type, int level);

// clears the files
void Map_clearFiles(Map* map);

/*-GETTERS--------------------------------------------------------------------*/
// returns the parent of the <tile> in <map> (either FileTile or MemTile)
MemTile* FileTile_getParent(Map* map, FileTile tile);
MemTile* MemTile_getParent(Map* map, MemTile *tile);

// get a copy of MemTile of map, level, at (col, row)
MemTile* Map_getMemTile(Map map, int level, int col, int row);

// get the RT1 array stored at <map>'s MemTile @ <level>, position <col, row>
RT1* Map_getMemTileRT1(Map map, int level, int col, int row);

//get the FileTile* at position <col, row>
FileTile* Map_getFileTile(Map map, int col, int row);

//get RT1 data for the FileTile at position <col, row>
RT1* Map_getFileTileRT1(Map map, int col, int row);

//get the number of columns of in the FileTile level of <map> 
int Map_getFileCol(Map map);

//get the number of rows of in the FileTile level of <map> 
int Map_getFileRow(Map map);

//get the width of a FileCell of <map>
float Map_getCellWidth(Map map);

//get the height of a cell of <map>
float Map_getCellHeight(Map map);

//get the min longitude of <map>
float Map_getMinLon(Map map);

//get the max longitude of <map>
float Map_getMaxLon(Map map);

//get the min latitude of <map>
float Map_getMinLat(Map map);

//get the max latitude of <map>
float Map_getMaxLat(Map map);


#endif
