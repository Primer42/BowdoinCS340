#include "FileTile.h"

#define TILE_DEBUG if(0)

/* construct & destroy ------------------------------------------------------ */
// creates and return a FileTile with filename <inFileName> and bounding lon/lat
FileTile* FileTile_new(char* inFileName,
		       int inMinLon, int inMinLat, int inMaxLon, int inMaxLat) {
  assert(inFileName);

  FileTile* newFileTile = malloc(sizeof(FileTile));
  strcpy(newFileTile->fileName, inFileName);
  newFileTile->fullBufferSize = (int)(FILE_BLOCK) / sizeof(RT1);
  newFileTile->buffer =
    (RT1*) malloc(sizeof(RT1) * newFileTile->fullBufferSize);
  //int i;
  //for(i = 0; i < newFileTile->fullBufferSize; i++) {
    //newFileTile->buffer[i].type = (char*) malloc(sizeof(char) * 4);
    //assert(newFileTile->buffer[i].type);
  //}
  newFileTile->bufferSize = 0;
  newFileTile->numRT1 = 0;

  newFileTile->minLon = inMinLon;
  newFileTile->minLat = inMinLat;
  newFileTile->maxLon = inMaxLon;
  newFileTile->maxLat = inMaxLon;

  // make sure file is clean
  fclose(FileTile_getFile(*newFileTile, "w+"));

  return newFileTile;
}

// fills the passed tile with the passed informaiton
void FileTile_fill(FileTile* tile, char* inFileName, 
		   int inMinLon, int inMinLat, int inMaxLon, int inMaxLat) {
  assert(tile); assert(inFileName);

  TILE_DEBUG{printf("fileName in FileTile_fillAndSetUp:\n%s\n\n", inFileName);}

  tile->fileName = (char*) malloc(sizeof(char)*(strlen(inFileName)+1));
  assert(tile->fileName);
  strcpy(tile->fileName, inFileName);
  tile->minLon = inMinLon;
  tile->minLat = inMinLat;
  tile->maxLon = inMaxLon;
  tile->maxLat = inMaxLat;

  tile->fullBufferSize = (int)(FILE_BLOCK) / sizeof(RT1);
  //printf("fullBufferSize=%d\n", tile->fullBufferSize);
  tile->buffer = (RT1*) malloc(sizeof(RT1) * tile->fullBufferSize);
  assert(tile->buffer);
  //int i;
  ///for(i = 0; i < tile->fullBufferSize; i++) {
  //tile->buffer[i].type = (char*) malloc(sizeof(char) * 4);
  //assert(tile->buffer[i].type);
  //}
  tile->bufferSize = 0;
  tile->numRT1 = 0;
}

// add RT1 into <tile>
void FileTile_addRT1(FileTile* tile, char* type,
		     int startLon, int startLat, int endLon, int endLat) {
  assert(tile); assert(type);

  TILE_DEBUG{
    printf("FileTile_addRT1: tile=%s ", tile->fileName); fflush(stdout);
  }

  //flush the buffer if it is at FULL_BUFFER_SIZE (full)
  if(tile->bufferSize+1 == tile->fullBufferSize) {
    FileTile_flushBuffer(tile);
  }
  
  //add to buffer
  FileTile_RT1ToBuffer(tile, type, startLon, startLat, endLon, endLat);
  
  //increment the number of RT1's stored
  tile->numRT1++;
  TILE_DEBUG{
    printf("FileTile_addRT1: done\n"); fflush(stdout);
  }
 
}

/*-BUFFER & FILE--------------------------------------------------------------*/
// addes <rt1> to buffer of <tile>
void FileTile_RT1ToBuffer(FileTile* tile, char* type,
			  int startLon, int startLat, int endLon, int endLat){
  assert(tile); assert(type);

  //printf("FileTile_RT1ToBuffer: enter "); 
  
  RT1* bufferRT1 = &(tile->buffer[tile->bufferSize]);
 
  //printf("tile=%d, tile buffer=%d, bufferRT1=%d\n", tile, tile->buffer, 
  //bufferRT1); 
  fflush(stdout); 
  
  //add all the data to the buffer
  RT1_fill(bufferRT1, type, startLon, startLat, endLon, endLat);

  //increment the bufferSize and numRT1
  tile->bufferSize++;
  //printf("FileTile_RT1ToBuffer: done\n"); fflush(stdout); 
}

// writes rest of RT1 in buffer to file
void FileTile_flushBuffer(FileTile* tile){
  assert(tile);

  FILE* flushFile = FileTile_getFile(*tile, "a");
  int i;
  for(i = 0; i < tile->bufferSize; i++)
    FileTile_RT1ToFile(flushFile, tile->buffer[i]);
  fclose(flushFile);

  //reset bufferSize
  tile->bufferSize = 0;
}

// writes <rt1> into  <file>
void FileTile_RT1ToFile(FILE* file, RT1 rt1){
  assert(file); assert(&rt1);

  //a line will read: <type> <start lon> <start lat> <end lon> <end lat>
  fprintf(file, "%s %d %d %d %d\n",
	  rt1.type, rt1.start.lon, rt1.start.lat, rt1.end.lon, rt1.end.lat);
}

/*-GETTERS--------------------------------------------------------------------*/
// creates and returns FILE* of <tile>
FILE* FileTile_getFile(FileTile tile, char* mode) {
  assert(mode);
  TILE_DEBUG{printf("opening file %s\n", tile.fileName); fflush(stdout);}
  FILE* tileFile = fopen(tile.fileName, mode); 
  assert(tileFile);
  return tileFile;
}

//returns the number of RT1s in <tile>
int FileTile_getNumRT1(FileTile tile) {
  return tile.numRT1;
}

// returns an array of RT1's that has all the RT1s in <tile>
RT1* FileTile_getRT1Array(FileTile tile) {
  RT1* data = (RT1*) malloc(tile.numRT1*sizeof(RT1)); assert(data);
  int i;
  //for(i = 0; i < tile.numRT1; i++) {
  //data[i].type = (char*) malloc(sizeof(char) * 4);
  //assert(data[i].type);
  // }
  
  FILE* tileFile = FileTile_getFile(tile, "r");
  char* fileLine = (char*) malloc(50*sizeof(char));
  assert(fileLine);

  char* inType;
  inType = (char*) malloc(4);  
  for(i = 0; i < tile.numRT1; i++) {
    fgets(fileLine, 50, tileFile);
   
    int inStartLon, inStartLat, inEndLon, inEndLat;
    sscanf(fileLine, "%s %d %d %d %d", inType,
	   &inStartLon, &inStartLat, &inEndLon, &inEndLat);

    RT1_fill(&data[i], inType, inStartLon, inStartLat,
	     inEndLon, inEndLat);
  }
  free(inType); 
  fclose(tileFile);
  return data;
}

// returns the minLon of <tile>
int FileTile_getMinLon(FileTile tile) {
  return tile.minLon;
}

// returns the minLat of <tile>
int FileTile_getMinLat(FileTile tile) {
  return tile.minLat;
}

// returns the maxLon of <tile>
int FileTile_getMaxLon(FileTile tile) {
  return tile.maxLon;
}

// returns the maxLat of <tile>
int FileTile_getMaxLat(FileTile tile) {
  return tile.maxLat;
}
