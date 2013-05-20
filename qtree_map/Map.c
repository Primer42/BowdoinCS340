#include "Map.h"

#define FILEMAP_DEBUG if(0)

/*-CONSTRUCT & DESTROY--------------------------------------------------------*/
// creates and return a Map
Map* Map_new(int clearFiles) {
  printf("Creating new Map: (%d,%d) with %d levels...\n",
	 MIN_LON, MIN_LAT, MEMTILE_LEVEL_COUNT);

  // new Map
  Map* newMap = (Map*) malloc(sizeof(Map)); assert(newMap);

  Map_initFileTiles(newMap);
  
  Map_initMemTiles(newMap);

  if(clearFiles)
    Map_clearFiles(newMap);

  return newMap;
}

// adds an RT1 to all the fileTiles in <map> that the RT1 goes through
void Map_addRT1(Map* map, char* type, int sLon, int sLat, int eLon, int eLat) {
  assert(map); assert(type);

  //make sure given RT1 is on <map>
  if(!(sLon<Map_getMinLon(*map) || eLon<Map_getMinLon(*map) ||
       sLat<Map_getMinLat(*map) || eLat<Map_getMinLat(*map) ||
       sLon>Map_getMaxLon(*map) || eLon>Map_getMaxLon(*map) ||
       sLat>Map_getMaxLat(*map) || eLat>Map_getMaxLat(*map))) {

    FILEMAP_DEBUG{
      printf("adding RT1 to start and end fileTiles\n"); fflush(stdout);
    }
    
    // find the row/col of both start and end point of RT1
    int sCol = (int) ((float) sLon - MIN_LON) / CELL_WIDTH;
    int sRow = (int) ((float) sLat - MIN_LAT) / CELL_HEIGHT;
    int eCol = (int) ((float) eLon - MIN_LON) / CELL_WIDTH;
    int eRow = (int) ((float) eLat - MIN_LAT) / CELL_HEIGHT;
    
    // make sure sCol <= eCol
    if(sCol > eCol) {
      int temp = sCol;
      sCol = eCol;
      eCol = temp;
    }

    // make sure sRow <= eRow
    if(sRow > eRow) {
      int temp = sRow;
      sRow = eRow;
      eRow = temp;
    }
    
    // add RT1 to all fileTiles bound by sCol/sRow/eCol/eRow
    // also add RT1 to all corrosponding MemTiles
    int col, row;
    for(col = sCol; col <= eCol; col++) {
      for(row = sRow; row <= eRow; row++) {
	FileTile_addRT1(&(map->fileTiles[col][row]),
			type, sLon, sLat, eLon, eLat);
	Map_addRT1toMemTiles(map, col, row,
			     type, sLon, sLat, eLon, eLat);
      }
    }
  }
}

// adds RT1 to corrosponding MemTiles
void Map_addRT1toMemTiles(Map* map, int col, int row,  char* type,
			  int sLon, int sLat, int eLon, int eLat) {

  //printf("Map_addRT1ToMemTiles: enter\n"); fflush(stdout);  
  assert(map); 
  // the number of parents that we need to add the RT1 to
  int numParentsToAdd = 0;

  // level 4 MemTile
  if((type[0] == 'A' && (type[1] == '4')) ||
      (type[0] == 'P' && (type[1] == '4')) ||
      (type[0] == 'F' && type[1] == '1')){
    numParentsToAdd = 1;
  }
  
  // level 3 MemTile
  if( (type[0] == 'B' && (type[1] == '2' || type[1] == '4')) ||
      (strcmp(type,"F18") == 0) ) {
    numParentsToAdd = 2;
  }
  
  // level 2 MemTile
  if((type[0] == 'A' && (type[1] == '2' || type[1] == '3')) ||
      (type[0] == 'P' && (type[1] == '2' || type[1] == '3')) ) {
    numParentsToAdd = 3;
  }
  
  // level 1 MemTile
  if((((type[0] == 'A' || type[0] == 'P') && type[1] == '1') ||
      strcmp(type, "B11") == 0 || strcmp(type, "B12") == 0 ||
      strcmp(type, "B13") == 0 || strcmp(type, "B19") == 0)) {
    numParentsToAdd = 4;
  }
  
  // level 0 MemTile
  if(strcmp(type, "F10") == 0 || 
     strcmp(type, "H01") == 0 || 
     //strcmp(type, "H02") == 0 ||
     strcmp(type, "H10") == 0 ||
     strcmp(type, "H30") == 0 ||
     strcmp(type, "H50") == 0 ) {
    numParentsToAdd = 5;
  }

  int i;
  int parentCol = col / 2;
  int parentRow = row / 2;
  int level = MEMTILE_LEVEL_COUNT - 1;
  MemTile* parent = &map->memTiles[level][parentCol][parentRow];
  assert(parent); 
  for(i = 0; i < numParentsToAdd && parent != NULL; i++) {
    MemTile_addRT1(parent, type, sLon, sLat, eLon, eLat);
    // get parent
    parentCol = parentCol / 2;
    parentRow = parentRow / 2;
    level--;
    if (level < 0) parent = NULL; 
    else { 
      parent = &map->memTiles[level][parentCol][parentRow];
      assert(parent);
    }
  }
}
 

/*-HELPERS--------------------------------------------------------------------*/
// initializes FileTiles in <map>
void Map_initFileTiles(Map* map) {
  assert(map);

  int col, row;
  map->fileTileCol = pow(2, MEMTILE_LEVEL_COUNT);
  map->fileTileRow = pow(2, MEMTILE_LEVEL_COUNT);
  char* fileName = (char*) malloc(30*sizeof(char)); assert(fileName);
  
  // malloc
  map->fileTiles = (FileTile**) malloc(map->fileTileCol*sizeof(FileTile*));
  assert(map->fileTiles);
  for(col = 0; col <  map->fileTileCol; col++) {
    map->fileTiles[col] =
      (FileTile*) malloc(map->fileTileRow*sizeof(FileTile));
    assert(map->fileTiles[col]);
  }

  // initialize
  int tileLat = MIN_LAT, tileLon = MIN_LON;
  for(row = 0; row < map->fileTileRow; row++) {
    for(col = 0; col <  map->fileTileCol; col++) {
      sprintf(fileName, "%s%d_%d", TILE_NAME, col, row);
      FileTile_fill(&map->fileTiles[col][row], fileName,
		    tileLon, tileLat,
		    tileLon + CELL_WIDTH, tileLat + CELL_HEIGHT);
      tileLon += CELL_WIDTH;
    }
    tileLon = MIN_LON;
    tileLat += CELL_HEIGHT;
  }
  free(fileName);

}

// initializes MemTiles in <map>
void Map_initMemTiles(Map* map) {
  assert(map);

  int col, row, level;
  map->memTileCol[0] = 1;
  map->memTileRow[0] = 1;

  for(level = 0; level < MEMTILE_LEVEL_COUNT; level++) {
 
    // set up cell width & height for this level
    int cellWidth = CELL_WIDTH * pow(2, MEMTILE_LEVEL_COUNT-level);
    int cellHeight = CELL_HEIGHT * pow(2, MEMTILE_LEVEL_COUNT-level);

    // malloc a row of pointers
    map->memTiles[level] =
      (MemTile**) malloc(map->memTileCol[level]*sizeof(MemTile*));
    assert(map->memTiles[level]);
    
    // malloc cols of MemTiles
    for(col = 0; col < map->memTileCol[level]; col++) {
      map->memTiles[level][col] = 
	(MemTile*) malloc(map->memTileRow[level]*sizeof(MemTile));
      assert(map->memTiles[level][col]);
    }

    // initialize
    for(row = 0; row < map->memTileRow[level]; row++) {
      for(col = 0; col < map->memTileCol[level]; col++) {
	MemTile_fill(&map->memTiles[level][col][row],
		     MIN_LON + col*cellWidth, MIN_LAT + row*cellHeight,
		     MIN_LON + (col+1)*cellWidth, MIN_LAT + (row+1)*cellHeight);
      }
    }
	    
    // set num of col/row for next level if it's not the last level
    if(level < MEMTILE_LEVEL_COUNT - 1) {
      map->memTileCol[level+1] = 2 * map->memTileCol[level];
      map->memTileRow[level+1] = 2 * map->memTileRow[level];
    }
  }
}

// decides if rt1 of type <type> needs to go into level <level>
int Map_typeInLevel(char* type, int level) {
  assert(type);
  
  // level 4 MemTile
  if(level >= 4 &&
     ((type[0] == 'A' && (type[1] == '4')) ||
      (type[0] == 'P' && (type[1] == '4')) ||
      (type[0] == 'F' && type[1] == '1'))){
    return 1;
  }
  
  // level 3 MemTile
  if(level >= 3 &&
     ((type[0] == 'B' && (type[1] == '2' || type[1] == '4')) ||
      (strcmp(type,"F18") == 0) )) {
    return 1;
  }
  
  // level 2 MemTile
  if(level >= 2 &&
     ((type[0] == 'A' && (type[1] == '2' || type[1] == '3')) ||
      (type[0] == 'P' && (type[1] == '2' || type[1] == '3')) )) {
    return 1;
  }
  
  // level 1 MemTile
  if(level >= 1 &&
     (((type[0] == 'A' || type[0] == 'P') && type[1] == '1') ||
      strcmp(type, "B11") == 0 || strcmp(type, "B12") == 0 ||
      strcmp(type, "B13") == 0 || strcmp(type, "B19") == 0)) {
    return 1;
  }
  
  // level 0 MemTile
  if(level >= 0 && 
     (strcmp(type, "F10") == 0 || 
      strcmp(type, "H01") == 0 || 
      //strcmp(type, "H02") == 0 ||
      strcmp(type, "H10") == 0 ||
      strcmp(type, "H30") == 0 ||
      strcmp(type, "H50") == 0 )) {
    return 1;
  }

  return 0;
  
}

// clears the files
void Map_clearFiles(Map* map) {
  assert(map);
  
  int col, row;
  for(row = 0; row < map->fileTileCol; row++)
    for(col = 0; col < map->fileTileRow; col++) {
      fclose(FileTile_getFile(map->fileTiles[col][row], "w+"));
    }
}

/*-GETTERS--------------------------------------------------------------------*/
// returns the parent of the <tile> in <map>
MemTile* FileTile_getParent(Map* map, FileTile tile) {
  
  //printf("getParent: map=%d ", map); fflush(stdout);
  assert(map); 
  // target MemTile's level, col, and row
  int level = 4;
  int col = (int) ((float) FileTile_getMinLon(tile) - MIN_LON) / 
    (CELL_WIDTH  *2);
  int row = (int) ((float) FileTile_getMinLat(tile) - MIN_LAT) / 
    (CELL_HEIGHT *2);
  //printf("col=%d, row=%d ", col, row); fflush(stdout);

  return &(map->memTiles[level][col][row]);
}

// returns the parent of the <tile> in <map>
MemTile* MemTile_getParent(Map* map, MemTile *tile) {
  assert(map); assert(tile);

  // target MemTile's level: one level above this level so -1
  int level = MEMTILE_LEVEL_COUNT - 1 - 
    log(MemTile_maxLat(*tile) - MemTile_minLat(*tile)) / log(2);

  if(level < 0)
    return tile;
  
  int col = (int) ((float) MemTile_minLon(*tile) - MIN_LON) / 
    pow(2, MEMTILE_LEVEL_COUNT - level)*CELL_WIDTH;
  int row = (int) ((float) MemTile_minLat(*tile) - MIN_LAT) / 
    pow(2, MEMTILE_LEVEL_COUNT - level)*CELL_WIDTH;
  
  return &(map->memTiles[level][col][row]);
}

// get a copy of MemTile of map, level, at (col, row)
MemTile* Map_getMemTile(Map map, int level, int col, int row) {
  assert(&map);

  return &map.memTiles[level][col][row];
}

/// get the RT1 array stored at <map>'s MemTile @ <level>, position <col, row>
RT1* Map_getMemTileRT1(Map map, int level, int col, int row) {
  assert(&map);

  return MemTile_getRT1(*Map_getMemTile(map, level, col, row));
}

//get the FileTile* at position <i,j>
FileTile* Map_getFileTile(Map map, int i, int j) {
  assert(&map);
  
  return &(map.fileTiles[i][j]);
}

//get RT1 data for the FileTile at position <col, row>
RT1* Map_getFileTileRT1(Map map, int col, int row) {
  assert(&map);

  return FileTile_getRT1Array(*Map_getFileTile(map, col, row));
}

//get the number of columns of in the FileTile level of <map> 
int Map_getFileCol(Map map) {
  assert(&map);
  return pow(2, MEMTILE_LEVEL_COUNT);
}

//get the number of rows of in the FileTile level of <map> 
int Map_getFileRow(Map map) {
  assert(&map);
  return pow(2, MEMTILE_LEVEL_COUNT);
}

//get the width of a FileCell of <map>
float Map_getCellWidth(Map map){
  assert(&map);
  return CELL_WIDTH;
}

//get the height of a cell of <map>
float Map_getCellHeight(Map map){
  assert(&map);
  return CELL_HEIGHT;
}

//get the min longitude of <map>
float Map_getMinLon(Map map){
  assert(&map);
  return MIN_LON;
}

//get the max longitude of <map>
float Map_getMaxLon(Map map){
  assert(&map);
  return MIN_LON + CELL_WIDTH*pow(2, MEMTILE_LEVEL_COUNT);
}

//get the min latitude of <map>
float Map_getMinLat(Map map){
  assert(&map);
  return MIN_LAT;
}

//get the max latitude of <map>
float Map_getMaxLat(Map map){
  assert(&map);
  return MIN_LAT + CELL_HEIGHT*pow(2, MEMTILE_LEVEL_COUNT);
}
