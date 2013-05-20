#ifndef __FileTile_h
#define __FileTile_h

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "RT1.h"

#define FILE_BLOCK (1<<18) //64 KB (note: this is 218)
#define RT1_LINE_LENGTH 46

typedef struct tile_t {
  char* fileName;
  int minLon, minLat, maxLon, maxLat;
  int numRT1;
  // buffer
  RT1* buffer;
  int bufferSize;
  int fullBufferSize;
} FileTile;

/*-CONSTRUCT & DESTROY--------------------------------------------------------*/
// creates and return a FileTile with filename <inFileName> and bounding lon/lat
FileTile* FileTile_new(char* inFileName,
		       int inMinLon, int inMinLat, int inMaxLon, int inMaxLat);

// fills the passed tile with the passed informaiton
void FileTile_fill(FileTile* tile, char* inFileName,
		   int inMinLon, int inMinLat, int inMaxLon, int inMaxLat);

// add RT1 into <tile>
void FileTile_addRT1(FileTile* tile, char* type,
		     int startLon, int startLat, int endLon, int endLat);

/*-BUFFER & FILE--------------------------------------------------------------*/
// addes <rt1> to buffer of <tile>
void FileTile_RT1ToBuffer(FileTile* tile, char* type, int startLon, int startLat,
		      int endLon, int endLat);

// writes <rt1> into file of <tile>
void FileTile_RT1ToFile(FILE* file, RT1 rt1);

// writes rest of RT1 in buffer to file
void FileTile_flushBuffer(FileTile* tile);

/*-GETTERS--------------------------------------------------------------------*/
// creates and returns FILE* of <tile>
FILE* FileTile_getFile(FileTile tile, char* mode);

//returns the number of RT1s in <tile>
int FileTile_getNumRT1(FileTile tile);

// returns an array of RT1's that has all the RT1s in <tile>
RT1* FileTile_getRT1Array(FileTile tile);

// returns the minLon of <tile>
int FileTile_getMinLon(FileTile tile);

// returns the minLat of <tile>
int FileTile_getMinLat(FileTile tile);

// returns the maxLon of <tile>
int FileTile_getMaxLon(FileTile tile);

// returns the maxLat of <tile>
int FileTile_getMaxLat(FileTile tile);

#endif
