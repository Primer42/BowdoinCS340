#ifndef __Reader_h
#define __Reader_h

#include <assert.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "FileTile.h"
#include "Map.h"

#define TIGER_LINE_LENGTH 250

#define TILE_DIRECTORY "DATA/"
#define TILE_INFO_FILE "tileInfo"

// creates a Map from data in <path>
Map* Reader_readAndCreate(char* dataPath);

// create the Map with existing data in directory DATA
Map* Reader_read();

/*-HELPERS--------------------------------------------------------------------*/
// process file at <filePath>: returns 1 if file opens, 0 if not
int Reader_readFile(Map* map, char* filePath);

// process <rt1> as RT1
void Reader_readLine(Map* map, char* rt1);

// process pre-constructed files in TILE_DIRECTORY
void Reader_readFileTiles(Map* map);

// creates memTiles from existing FileTiles in <map>
void Reader_createMemTiles(Map* map);

#endif
