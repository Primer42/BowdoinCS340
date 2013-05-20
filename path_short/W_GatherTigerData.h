/* W_TigerStructs.h

Defines the various spefic stuctured to the Tiger data
*/

#ifndef __W_TigerStructs_h
#define __W_TigerStructs_h

#include <dirent.h>
#include <stdio.h>
#include <sys/types.h> 

typedef struct _node {
  int longitude;
  int latitude;
}Node;

typedef struct _point {
  int longitude;
  int latitude;
}Point;

typedef struct _chain{
  int TLID;
  char CFCC[3];
  Node start;
  Node end;
  Point* points;
  int numPoints;
}Chain;

typedef struct _county{
  int countyID;
  Chain** chains;
  int numChains;

  int maxLat;
  int maxLong;
  int minLat;
  int minLong;
}County;

typedef struct _state{
  County* counties;
  int numCounties;
}State;

typedef struct _tigerData{
  State* states;
  int numStates;
}TigerData;

//compare function for qsort and bsearch by TLID
int compareChainsTLID(const void* v1, const void* v2);

//compare function for qsort and bsearch by start Node
int compareChainsStart(const void* v1, const void* v2);

//compare function for qsort and bsearch by end Node
int compareChainsEnd(const void* v1, const void* v2);

//Gets the data from the various Tiger Files
TigerData* getTigerFilesData(char* path);

//Gets data specifically from a state folder
void getDataFromStateFolder(DIR* stateDir, char* statePath, State* currState, 
							struct dirent* countyDirent);

//Gets data from the COUNTSnn file
void getCountData(FILE* countFile, State* currState);

//Gets data from a given county folder
void getDataFromCountyFolder(DIR* stateDir, char* statePath, State* currState,
							 struct dirent* countyDirent);

//Opens a file from a path and a dirent
FILE* openDataFile(char* countyFolderPath, struct dirent* fileDirent);

//Get data from a .RT1 file
void getDataFromRT1(FILE* file, County* county);

//Get data from a .RT2 file
void getDataFromRT2(FILE* file, County* county);

//Figures out the max and min long, lat cooridinates
void determineMaxAndMinLatLong(County* county, Node* startNode, Node* endNode, 
							   int firstTime);

#endif 
