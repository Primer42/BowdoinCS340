/*W_TigerInput.c

Gets information from Tiger files for use in other programs
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/dirent.h>
#include <sys/dir.h>
#include <string.h>
#include "W_GatherTigerData.h"

#define TIGER_DEBUG if(1)
#define TIGER_DEBUG_RT2 if(0)
#define TIGER_DEBUG_COUNT if(0)

//max total num points = 50,201,081
//max total num chains = 6,677,589
//states attempted: CT, DC, DE, MA, MD, ME, NH, NJ, NY, PA, RI, VT
int totalNumPoints=0;  int totalNumChains=0;


//compare chains function for qsort and bsearch
int compareChainsTLID(const void* v1, const void* v2){
  int t;
  Chain* c1 = *(Chain**)v1;
  Chain* c2 = *(Chain**)v2;
  if(c1->TLID < c2->TLID) t = -1;
  else if (c1->TLID > c2->TLID) t = 1;
  else t = 0;
  return t;
}

//compare chains by start node- allows for sorting by start
int compareChainsStart(const void* v1, const void* v2) {
  assert(v1); assert(v2);

  Chain* c1 = *(Chain**)v1;
  Chain* c2 = *(Chain**)v2;
  
  if(c1->start.longitude > c2->start.longitude)
    return 1;
  
  if(c1->start.longitude == c2->start.longitude) {
    if(c1->start.latitude > c2->start.latitude)
      return 1;
    if(c1->start.latitude < c2->start.latitude)
      return -1;
    if(c1->start.latitude == c2->start.latitude)
      return 0;
  }
  
  return -1;
}

//compare chains by end node- allows for sorting by end
int compareChainsEnd(const void* v1, const void* v2) {
  assert(v1); assert(v2);
  
  Chain* c1 = *(Chain**)v1;
  Chain* c2 = *(Chain**)v2;
  
  if(c1->end.longitude > c2->end.longitude)
    return 1;
  
  if(c1->end.longitude == c2->end.longitude) {
    if(c1->end.latitude > c2->end.latitude)
      return 1;
    if(c1->end.latitude < c2->end.latitude)
      return -1;
    if(c1->end.latitude == c2->end.latitude)
      return 0;
  }
  
  return -1;
}

// ************************************************************
TigerData* getTigerFilesData(char* basePath){
  TIGER_DEBUG{printf("starting getTigerFilesData()\n"); fflush(stdout);}
  DIR* tigerDir;  
  //  char* basePath = "Tiger_Files";//this is the directory where all the tiger files are
  tigerDir = opendir(basePath);
  assert(tigerDir);

  //will allow us to go through the tigerDir directory
  struct dirent* stateDirent;

  //will hold all the data in states, counties and chains
  TigerData* data;
  data = (TigerData*)malloc(sizeof(TigerData));
  assert(data);

  TIGER_DEBUG{printf("determining the number of states\n"); fflush(stdout);}
  data->numStates = 0;
  //each folder will hold one state- for each readdir add one to numstates
  while((stateDirent=readdir(tigerDir))!=NULL){
    if(stateDirent->d_name[0] != '.'){
      data->numStates++;
    }
  }
  TIGER_DEBUG{printf("num states=%d\n", data->numStates); fflush(stdout);}
  rewinddir(tigerDir);//go back to the beginning of the dir- we need to go through it again

  //now that we know how many states there are, we can malloc the states array
  data->states = (State*)malloc((data->numStates)*sizeof(State));
  assert(data->states);

  //starting to go through each state folder and get the information from it
  int currStateIndex;
  DIR* stateDir;

  for(currStateIndex = 0; currStateIndex < data->numStates; currStateIndex++){
    State* currState;
    currState = &data->states[currStateIndex];

    stateDirent=readdir(tigerDir);
    while(stateDirent->d_name[0] == '.'){
      stateDirent=readdir(tigerDir);
    }
    TIGER_DEBUG{printf("going to state %s\n", stateDirent->d_name); fflush(stdout);}

    //make a char* with the path for the state folder
    char currStatePath[strlen(basePath)+strlen(stateDirent->d_name)+2];
    sprintf(currStatePath, "%s/%s", basePath, stateDirent->d_name);
    stateDir = opendir(currStatePath);
    assert(stateDir);

    struct dirent* countyDirent;
    
    //using function to get data from the state folder specified
    TIGER_DEBUG{printf("going into getDatFromStateFolder()\n"); fflush(stdout);}
    getDataFromStateFolder(stateDir, currStatePath, currState, countyDirent);
    
  }
  //close open dir- we don't need them anymore
  closedir(stateDir);
  closedir(tigerDir);

  TIGER_DEBUG{printf("returning data\n"); fflush(stdout);}
  return data;
}





/* ************************************************************ */
void getDataFromStateFolder(DIR* stateDir, char* statePath, State* currState, struct dirent* countyDirent){
  TIGER_DEBUG{printf("starting getDataFromStateFolder()\n"); fflush(stdout);}
    
  //skip the hidden files starting with '.'- won't be helpful
  countyDirent = readdir(stateDir);
  while((countyDirent->d_name[0] == '.') || (countyDirent->d_name[0] == 'T')){
    TIGER_DEBUG{printf("currently looking at:%s\n", countyDirent->d_name); fflush(stdout);}
    countyDirent = readdir(stateDir);
	//countyDirent should now be pointing to the COUNTSnn file.
  }
  TIGER_DEBUG{printf("starting to read from the state folder- county dirent pointing at COUNT file\n"); fflush(stdout);}

  char countFileLocation[strlen(statePath)+strlen(countyDirent->d_name)+2];
  sprintf(countFileLocation, "%s/%s", statePath, countyDirent->d_name);
  //use the COUNTSnn file to set up arrays
  FILE* countFile;
  countFile = fopen(countFileLocation, "r");
  assert(countFile);

  TIGER_DEBUG{printf("COUNT file opened from %s\nStarting to read data from COUNT file into numCounties for the current state\n", countFileLocation); fflush(stdout);}

  //use function to get data from the COUNTSnn file
  getCountData(countFile, currState);
  fclose(countFile);

  rewinddir(stateDir);

  TIGER_DEBUG{printf("about to run getDataFromCountyFolder()\n"); fflush(stdout);}
  //now that we have the count data, go into the county folder and get
  //the data from there
  getDataFromCountyFolder(stateDir, statePath, currState, countyDirent);
}








//uses the COUNTSnn.txt file to determine the number of counties and
//chains per county
void getCountData(FILE* countFile, State* currState){
  TIGER_DEBUG{printf("starting to get count data using getCountData function\n"); fflush(stdout);}
  
  //store a given line of the file as a char*
  char countLine[46];
  assert(countLine);
  currState->numCounties = 0;
  while(fgets(countLine, 46, countFile) != NULL){
    //when point 32 in the line ==1, it is refering to an RT1 file.  This means its a county, and we need to add 1 to the number of counties
    if(atoi(&countLine[32]) == 1){
      currState->numCounties++;
    }
  }
  rewind(countFile);
  
  currState->counties = (malloc(sizeof(County) * (currState->numCounties)));
  assert(currState->counties);
    
  TIGER_DEBUG{printf("numCounties is %d\n", currState->numCounties); fflush(stdout);}
  //go through each county, and mallco the appropriate sized chain array, the right number of chains, and the County ID number
  int currCountyIndex;
  TIGER_DEBUG{printf("setting up chain arrays for each county\n"); fflush(stdout);}

  currCountyIndex = 0;
  while(fgets(countLine, 46, countFile) != NULL){
    if(atoi(&countLine[32]) == 1){
      currState->counties[currCountyIndex].countyID = atoi(&countLine[19]);
      TIGER_DEBUG{printf("countyID = %d\n", currState->counties[currCountyIndex].countyID); fflush(stdout);}
      currState->counties[currCountyIndex].numChains = atoi(&countLine[37]);
      currState->counties[currCountyIndex].chains = (Chain**)malloc((currState->counties[currCountyIndex].numChains)*sizeof(Chain*));
      int i;
      for(i = 0; i < currState->counties[currCountyIndex].numChains; i++)
	currState->counties[currCountyIndex].chains[i] = 
	  (Chain*) malloc(sizeof(Chain));
      assert(currState->counties[currCountyIndex].chains);
      TIGER_DEBUG{printf("numChains in COUNT method for county %d =%d\n", currCountyIndex, currState->counties[currCountyIndex].numChains); fflush(stdout);}

      TIGER_DEBUG_COUNT{totalNumChains += atoi(&countLine[37]); printf("current total numChains = %d\n", totalNumChains); fflush(stdout);}

      currCountyIndex++;
    }
  }

  TIGER_DEBUG{printf("finished setting up chain arrays for each county\n"); 
  fflush(stdout);}

}




//goes into a county folder and get the data from it.
void getDataFromCountyFolder(DIR* stateDir, char* statePath, State* currState, struct dirent* countyDirent){

  TIGER_DEBUG{printf("starting getDataFromCountyFolder()\n"); fflush(stdout);}
  
  int currCountyIndex;
  for(currCountyIndex = 0; currCountyIndex < currState->numCounties; currCountyIndex++){
    
    TIGER_DEBUG{printf("looking at county number %d\n", currCountyIndex); fflush(stdout);}
    countyDirent = readdir(stateDir);
    TIGER_DEBUG{printf("checking file %s\n", countyDirent->d_name); fflush(stdout);}
    while(countyDirent->d_name[0] == '.' ||  
	  countyDirent->d_name[0] != 'T' || 
	  countyDirent->d_name[0] == 'C'){
      
      //county dirent now points to a county folder
      countyDirent = readdir(stateDir);
      assert(countyDirent);
      TIGER_DEBUG{printf("checking file %s\n", countyDirent->d_name); fflush(stdout);}
    }

    struct dirent* fileDirent;
    DIR* fileDir;
    char countyFolderPath[strlen(statePath)+strlen(countyDirent->d_name)+2];
    sprintf(countyFolderPath, "%s/%s", statePath, countyDirent->d_name);
    TIGER_DEBUG{printf("looking at %s\n", countyFolderPath); fflush(stdout);}
    fileDir = opendir(countyFolderPath);
    assert(fileDir);

    County* currCounty;
    int i;
    for(i = 0; i < currState->numCounties; i++){
      if(currState->counties[i].countyID == atoi(&countyDirent->d_name[5])){
	currCounty = &currState->counties[i];
	break;
      }
    }

    TIGER_DEBUG{printf("county ID = %d\n", currCounty->countyID); fflush(stdout);}
        
    FILE* currFile;
    int gotRT1;
    gotRT1 = 0;
    while((fileDirent = readdir(fileDir))!=NULL){
      TIGER_DEBUG{printf("fileDirent = %s\t", fileDirent->d_name); fflush(stdout);}
      if(fileDirent->d_name[0] == '.' || fileDirent->d_name[9] != 'R'){
	//fileDirent = readdir(fileDir);
	TIGER_DEBUG{printf("we don't want this file\n"); fflush(stdout);}
      }
      else{
	TIGER_DEBUG{printf("got a data file want.\n"); fflush(stdout);}
	//TIGER_DEBUG{printf("looking at %s\n", filePath); fflush(stdout);}
	
	//TIGER_DEBUG{printf("got a data file- extracting data (if we want it)\n"); fflush(stdout);}
	if(!gotRT1) {
	  if(fileDirent->d_name[11] == '1') {
	    currFile = openDataFile(countyFolderPath, fileDirent);//open the file- I only do this within the 
	    TIGER_DEBUG{printf("getting data from RT1 file\n"); fflush(stdout);}
	    getDataFromRT1(currFile, currCounty);
	  
	    TIGER_DEBUG{printf("sorting RT1 data\n"); fflush(stdout);}
	    qsort(currCounty->chains, currCounty->numChains, sizeof(Chain*), compareChainsTLID);
	    rewinddir(fileDir);
	    gotRT1 = 1;
	    fclose(currFile);
	  }
	}
	else{
	  switch(fileDirent->d_name[11]){//this should specify the Record Type
	    //if you want the file, remember to open the file using openDataFile(countyFolderPath, fileDirent);
	  case '2':
	    TIGER_DEBUG{printf("about to get RT2 data from file\n"); fflush(stdout);}
	    currFile = openDataFile(countyFolderPath, fileDirent);
	    getDataFromRT2(currFile, currCounty);
	  }
	  fclose(currFile);
	}
      } 
    }
    free(fileDirent);
    closedir(fileDir);
  }
}



//Opens a file from a path and a dirent
FILE* openDataFile(char* countyFolderPath, struct dirent* fileDirent){
  FILE* dataFile;
  char filePath[strlen(countyFolderPath)+strlen(fileDirent->d_name)+2];
  sprintf(filePath, "%s/%s", countyFolderPath, fileDirent->d_name);
  dataFile = fopen(filePath, "r");
  assert(dataFile);
  return dataFile;
}


//Get data from a .RT1 file
void getDataFromRT1(FILE* file, County* county){

  TIGER_DEBUG{printf("starting getDataFromRT1() function\n"); fflush(stdout);}

  char currLine[232];

  int i;
  for(i = 0; (fgets(currLine, 232, file)) != NULL; i++){
    sscanf(&currLine[5], "%d", &county->chains[i]->TLID);//get the TLID from the 6th char of the line

    sscanf(&currLine[55], "%s", county->chains[i]->CFCC);//get the CFCC from the 56th character of the line
    Node startNode, endNode;
    sscanf(&currLine[190], "%d %d %d %d", &(startNode.longitude), &(startNode.latitude), &(endNode.longitude), &(endNode.latitude));

    determineMaxAndMinLatLong(county, &startNode, &endNode, i);

    county->chains[i]->start = startNode;
    county->chains[i]->end = endNode;
    county->chains[i]->numPoints = 0;

    TIGER_DEBUG_COUNT{totalNumPoints += 2; printf("total numPoints = %d\n", totalNumPoints); fflush(stdout);}

    //TIGER_DEBUG{printf("filled chain\n", currLine); fflush(stdout);}
  }
  TIGER_DEBUG{printf("finished getting data from RT1 file\n"); fflush(stdout);}
}



//Get data from a .RT2 file
void getDataFromRT2(FILE* file, County* county){

  TIGER_DEBUG{printf("starting to get data from RT2\n"); fflush(stdout);}
  char currLine[211];
  int finalLine;

  finalLine = 0;
  
  while((fgets(currLine, 211, file))!=NULL && !finalLine){
    TIGER_DEBUG_RT2{printf("looking for RT2 data in line:\n%s", currLine); fflush(stdout);}
    int callsToFGets;
    callsToFGets=1;
    Chain* currChain;
    int TLIDkey;
    TLIDkey = atoi(&currLine[5]);
    TIGER_DEBUG{printf("looking for TLID %d\t", TLIDkey); fflush(stdout);}
    Chain* lookingForChain;
  
    lookingForChain = (Chain*)malloc(sizeof(Chain));
    
    lookingForChain->TLID = TLIDkey;
    Chain** result= (Chain**)bsearch(&lookingForChain, county->chains, (county->numChains), sizeof(Chain*), compareChainsTLID);
    
    if(result!=NULL){
      currChain = *result;
      TIGER_DEBUG{printf("found chain TLID=%d\n", currChain->TLID); fflush(stdout);}
	  
      while((fgets(currLine, 211, file))!=NULL){
	
		TIGER_DEBUG_RT2{printf("new line's TLID = %d\t RSQ = %d\n", atoi(&currLine[5]), atoi(&currLine[15])); fflush(stdout);}
		if(atoi(&currLine[15]) == 1){
		  break;
		}
		callsToFGets++;
      }
      TIGER_DEBUG_RT2{printf("now at line: \n%s", currLine); fflush(stdout);}
      if(atoi(&currLine[5]) != currChain->TLID){
		
		TIGER_DEBUG_RT2{printf("rewinding 1 line\n"); fflush(stdout);}
		
		fseek(file, -2*sizeof(char)*210, SEEK_CUR);
		fgets(currLine, 211, file);
		
		TIGER_DEBUG_RT2{printf("now at line (after rewind and an fgets): \n%s", currLine); fflush(stdout);}
      }
      else{
	finalLine = 1;
      }

      //the number of lines matching TLID will be callsToFGets.
      TIGER_DEBUG_RT2{printf("found %d lines for this TLID\n", callsToFGets); fflush(stdout);}

      TIGER_DEBUG_RT2{printf("determine # points on line\n"); fflush(stdout);}
      int cursorPos;
      cursorPos = 18;//start at location for Long1
      int numPointsLocal;
      numPointsLocal = 10*(callsToFGets-1);//10 points for each full line

      TIGER_DEBUG_RT2{printf("numPoints from before = %d\n", numPointsLocal); fflush(stdout);}
      while(atoi(&currLine[cursorPos]) != 0){
		cursorPos+=20;
		numPointsLocal++;
      }
	  
      currChain->numPoints = numPointsLocal;
      currChain->points = (Point*)malloc((currChain->numPoints)*sizeof(Point));
      assert(currChain->points);

      TIGER_DEBUG_COUNT{totalNumPoints += numPointsLocal;printf("total numPoints = %d\n", totalNumPoints);fflush(stdout);}

      TIGER_DEBUG{printf("%d points total for this TLID\n", currChain->numPoints); fflush(stdout);}
      
      fseek(file, -1*(callsToFGets)*(sizeof(char)*210), SEEK_CUR);
      fgets(currLine, 211, file);
      TIGER_DEBUG_RT2{printf("back at first line:\n%s\n", currLine); fflush(stdout);}
      
      int currPoint;
      for(currPoint = 0, cursorPos = 18; currPoint < currChain->numPoints; currPoint++, cursorPos+=19){
	if(cursorPos > 207){
	  fgets(currLine, 211, file);
	  cursorPos = 18;
	}
	sscanf(&currLine[cursorPos], "%d%d", &currChain->points[currPoint].longitude, &currChain->points[currPoint].latitude);
	TIGER_DEBUG_RT2{printf("getting point %d, %d\n", currChain->points[currPoint].longitude, currChain->points[currPoint].latitude); fflush(stdout);}
      }
      TIGER_DEBUG_RT2{printf("got all points- moving on\n"); fflush(stdout);}
    }
  }
  TIGER_DEBUG{printf("finished getting data from RT2 file\n"); fflush(stdout);}
}


//Figures out the max and min long, lat cooridinates
void determineMaxAndMinLatLong(County* county, Node* startNode, Node* endNode, int firstTime){
  
  if(firstTime == 0){
    TIGER_DEBUG{printf("this is first time\n"); fflush(stdout);}
    if(startNode->longitude > endNode->longitude){
      county->maxLong = startNode->longitude;
      county->minLong = endNode->longitude;
    }
    else{
      county->minLong = startNode->longitude;
      county->maxLong = endNode->longitude;
    }

    if(startNode->latitude > endNode->latitude){
      county->maxLat = startNode->latitude;
      county->minLat = endNode->latitude;
    }
    else{
      county->minLat = startNode->latitude;
      county->maxLat = endNode->latitude;
    }
  }
  else{
    if(startNode->longitude > county->maxLong){
      county->maxLong = startNode->longitude;}

    if(endNode->longitude > county->maxLong){
      county->maxLong = endNode->longitude;}

    if(startNode->latitude > county->maxLat){
      county->maxLat = startNode->latitude;}

    if(endNode->latitude > county->maxLat){
      county->maxLat = endNode->latitude;}

    if(startNode->longitude < county->minLong){
      county->minLong = startNode->longitude;}

    if(endNode->longitude < county->minLong){
      county->minLong = endNode->longitude;}

    if(startNode->latitude < county->minLat){
      county->minLat = startNode->latitude;}

    if(endNode->latitude < county->minLat){
      county->minLat = endNode->latitude;}
  }
}
