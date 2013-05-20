#include "Reader.h"

#define READER_DEBUG if(0)

// creates a Map from data in <path>
Map* Reader_readAndCreate(char* dataPath) {
  assert(dataPath);

  printf("making newMap\n"); fflush(stdout);

  // clears the tileInfo.txt
  char* temp = (char*) malloc((strlen(TILE_DIRECTORY) + strlen(TILE_INFO_FILE))*
			      sizeof(char)); assert(temp);
  sprintf(temp, "%s%s", TILE_DIRECTORY, TILE_INFO_FILE);
  FILE* tileInfo = fopen(temp, "w");
  assert(tileInfo);
  free(temp);
  
  //make the Map*
  Map* newMap = Map_new(1);
  READER_DEBUG{
    printf("open data directory\n"); fflush(stdout);
  }
  


  // storage for total RT1 on map
  int mapTotal = 0;

  //open data directory
  DIR* dataDir = opendir(dataPath);
  struct dirent* curFileDirent;

  //if file in directory is of type ??.RT1 open and read file
  char* filePath = (char*) malloc(sizeof(char) * (strlen(dataPath)+7));
  assert(filePath);
  for(curFileDirent = readdir(dataDir);
      curFileDirent != NULL; curFileDirent = readdir(dataDir)) {
    printf("looking at %s", curFileDirent->d_name); fflush(stdout);
    if(curFileDirent->d_name[5] == '1') {
      READER_DEBUG{printf("reading the file"); fflush(stdout);}
      sprintf(filePath,"%s%s", dataPath, curFileDirent->d_name);
      filePath[strlen(dataPath) + 6] == '\0';
      
      mapTotal += Reader_readFile(newMap, filePath);
    }
    printf("\n"); fflush(stdout); 
  }
  
  //once all files have been read, flush all the tiles.
  int i, j;
  for(i = 0; i < Map_getFileRow(*newMap); i++){
    for(j = 0; j < Map_getFileCol(*newMap); j++){
      FileTile_flushBuffer(&newMap->fileTiles[i][j]);
    }
  }

  //output tileInfo: total RT1's on map
  int tileTotal = 0;
  for(i = 0; i < Map_getFileRow(*newMap); i++){
    for(j = 0; j < Map_getFileCol(*newMap); j++) {
      tileTotal += newMap->fileTiles[i][j].numRT1;
      fprintf(tileInfo, "[%d,%d]\t%d\n",
	      i, j, newMap->fileTiles[i][j].numRT1); 
    }
  }
  fprintf(tileInfo, "\nn: %d\n", mapTotal);
  fprintf(tileInfo, "n_t: %d\n", tileTotal);
  fprintf(tileInfo, "n_t/n: %f\n", (double)(tileTotal)/mapTotal);
  fclose(tileInfo);
	 
  free(filePath);
	 
  return newMap;
}

// create the Map with existing data in directory DATA
Map* Reader_read() {
  printf("creating MemTiles from FileTiles that's in %s", TILE_DIRECTORY);
  fflush(stdout);

  // make the Map*
  Map* newMap = Map_new(0);
  
  Reader_readFileTiles(newMap);

  Reader_createMemTiles(newMap);
  
  return newMap;
}

// process file at <filePath>: returns total RT1 added
int Reader_readFile(Map* map, char* filePath){
  assert(map); assert(filePath);

  //open .RT1 file
  FILE* rt1File = fopen(filePath, "r");
  if(rt1File == NULL){
    printf("unable to open file");
    fflush(stdout);
  }
  //storage variables
  char* rt1Line = (char*) malloc(sizeof(char) * TIGER_LINE_LENGTH);
  assert(rt1Line);
  int lineCounter = 0;
  //read in line and process it until end of file
  while(fgets(rt1Line, TIGER_LINE_LENGTH, rt1File) != NULL){
    
    Reader_readLine(map, rt1Line);

    lineCounter++;
    if(lineCounter%50000==1) {printf("."); fflush(stdout);}
  }

  // writing num of RT1's per state
  char* tileInfoPath =
    (char*) malloc((strlen(TILE_DIRECTORY) + strlen(TILE_INFO_FILE)) * 
		   sizeof(char)); assert(tileInfoPath);
  sprintf(tileInfoPath, "%s%s", TILE_DIRECTORY, TILE_INFO_FILE);
  FILE* tileInfo = fopen(tileInfoPath, "a");
  assert(tileInfo);
  fprintf(tileInfo, "%c%c\t%d\n", filePath[strlen(filePath)-6], 
	  filePath[strlen(filePath)-5], lineCounter);
  fclose(tileInfo);

  free(rt1Line);
  fclose(rt1File);
  return lineCounter;
}

// process <rt1> as RT1: returns 1 if <rt1>'s data added, 0 if not
void Reader_readLine(Map* map, char* rt1){
  assert(map); assert(rt1);
  //storage variables
  char type[4];
  int startLon, startLat, endLon, endLat;
  
  //setup data
  strncpy(type, &rt1[55], 3);
  type[3] = '\0';
  sscanf(&rt1[190], "%d%d%d%d", &startLon, &startLat, &endLon, &endLat);
  
  //add RT1 data to <map>
  Map_addRT1(map, type, startLon, startLat, endLon, endLat);
}

// process pre-constructed files in TILE_DIRECTORY
void Reader_readFileTiles(Map* map) {
  assert(map);

  int row, col;
  for(row = 0; row < Map_getFileRow(*map); row++){
    for(col = 0; col < Map_getFileCol(*map); col++){
      FileTile* tile = Map_getFileTile(*map, col, row);

      FILE* tileFile = FileTile_getFile(*tile, "r");
      assert(tileFile);
      rewind(tileFile);
      char* tileFileLine = (char*) malloc(50*sizeof(char));

      int numRT1 = 0;
      while(fgets(tileFileLine, 50, tileFile) != NULL) {
	numRT1++;
      }
      tile->numRT1 = numRT1;
      printf("readFileTiles: %s: %d\n", tile->fileName, tile->numRT1); fflush(stdout);

      fclose(tileFile);
      free(tileFileLine);
    }
  }
}

// creates memTiles from existing FileTiles in <map>
void Reader_createMemTiles(Map* map) {
  assert(map);
  printf("Constructing MemTiles:\n"); fflush(stdout);
  
  // construct more detailed (level 4) MemTiles
  int level = MEMTILE_LEVEL_COUNT - 1, col, row;
  printf("\tLevel %d", level); fflush(stdout);
  for(row = 0; row < Map_getFileRow(*map); row++) {
    for(col = 0; col < Map_getFileCol(*map); col++) {
      // target
      MemTile* tileM = Map_getMemTile(*map, level, col/2, row/2); assert(tileM);
      // source
      FileTile* tileF = Map_getFileTile(*map, col, row); assert(tileF);
      RT1* tileFData = FileTile_getRT1Array(*tileF); assert(tileFData);
      int tileFSize = FileTile_getNumRT1(*tileF);

      char* type =
	(char*) malloc(sizeof(RT1_getType(&tileFData[0])));
      assert(type);
      // check if rt1 type needs to be added into tileM
      int i, startLon, startLat, endLon, endLat;
      for(i = 0; i < tileFSize; i++) {
	strcpy(type, RT1_getType(&tileFData[i]));
	startLon = Node_getLon(RT1_getStart(tileFData[i]));
	startLat = Node_getLat(RT1_getStart(tileFData[i]));
	endLon = Node_getLon(RT1_getEnd(tileFData[i]));
	endLat = Node_getLat(RT1_getEnd(tileFData[i]));
	if(Map_typeInLevel(type, level)) {
	  MemTile_addRT1(tileM, type, startLon, startLat, endLon, endLat);      
	}
      }
      free(type);
      free(tileFData);
    }
    printf("."); fflush(stdout);
  }
  printf("done\n"); fflush(stdout);

  // construct rest of MemTiles
  for(; level > 0; level--) {
    printf("\tLevel %d", level-1); fflush(stdout);
    for(row = 0; row < map->memTileRow[level]; row++) {
      for(col = 0; col < map->memTileCol[level]; col++) {
	//target
	MemTile* target =
	  Map_getMemTile(*map, level - 1, col/2, row/2); assert(target);
	//source
	MemTile* source = 
	  Map_getMemTile(*map, level, col, row); assert(source);

	RT1* data = MemTile_getRT1(*source); assert(data);
	int dataSize = MemTile_dataSize(*source);

	char* type =
	  (char*) malloc(sizeof(RT1_getType(&data[0])));
	assert(type);
	int i, startLon, startLat, endLon, endLat;
	for(i = 0; i < dataSize; i++) {
	  strcpy(type, RT1_getType(&data[i]));
	  startLon = Node_getLon(RT1_getStart(data[i]));
	  startLat = Node_getLat(RT1_getStart(data[i]));
	  endLon = Node_getLon(RT1_getEnd(data[i]));
	  endLat = Node_getLat(RT1_getEnd(data[i]));
	  if(Map_typeInLevel(type, level-1)) {
	    MemTile_addRT1(target, type, startLon, startLat, endLon, endLat); 
	  }
	}
	free(type);
      }
      printf("."); fflush(stdout);
    }
    printf("done\n"); fflush(stdout);
  }
  printf("MemTiles construction - complete\n\n"); fflush(stdout);
}
