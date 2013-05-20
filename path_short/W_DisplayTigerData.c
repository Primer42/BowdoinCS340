/*W_DisplayTigerData.c

Displays data gathered by W_GatherTigerData.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "W_GatherTigerData.h"
#include "BW_LinkedVerticies.h"
#include "BW_DArray.h"
#include "BW_FindPath.h"
#include "BW_Path.h"
#include "pqheap.h"


#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif 

#define TIGER_DISPLAY_DEBUG if(1)

GLfloat red[3] = {1.0, 0.0, 0.0};
GLfloat green[3] = {0.0, 1.0, 0.0};
GLfloat blue[3] = {0.0, 0.0, 1.0};
GLfloat black[3] = {0.0, 0.0, 0.0};
GLfloat white[3] = {1.0, 1.0, 1.0};
GLfloat gray[3] = {0.5, 0.5, 0.5};
GLfloat yellow[3] = {1.0, 1.0, 0.0};
GLfloat magenta[3] = {1.0, 0.0, 1.0};
GLfloat cyan[3] = {0.0, 1.0, 1.0};

GLdouble eye[2] = {0.0, 0.0};
GLfloat zoomFactor = 0.0;

GLint overallMaxLong, overallMaxLat, overallMinLong, overallMinLat;
GLfloat midLat, midLong;
GLfloat ruler; //will hold the lat/long distance to compare all other distances againt to get a uniform disply of points
GLfloat windowWidth;
GLfloat viewRightSide, viewLeftSide, viewTopSide, viewBottomSide;

TigerData* data;
DArray* verticies;

GLint showRoad = 1;
GLint showHydro = 1;
GLint showRail = 1; 
GLint showNonVisible = 1;
GLint showPhysical = 1;

GLint showDetail;

Vertex* startVertex;
Vertex* endVertex;
Vertex* DUMMY_VERTEX;
#define DUMMY_ANGLE 400000000


void display(void);
void init(void);
void keypress(unsigned char key, int x, int y);
void main_menu(int value);

void determineLatLongScale();
float* getVerticesFromNode(Node* node);
void drawChain(Chain* chain);
void changeLookingAt (void);
void selectPoint(int button, int state, int x, int y);
int findBoxMin(int key, int low, int high); 


int main(int argc, char** argv){
  /* open a window */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(500, 500);
  glutInitWindowPosition(100,100);
  glutCreateWindow(argv[0]);
  
  /* OpenGL init */
  glClearColor(1.0, 1.0, 1.0, 0);   /* set background color white*/

  //if a file is not passed correctly, display the proper usage and exit
  if(argc <2) {
    printf("usage: W_DisplayTigerData <data path>\n");
    exit(1);
  }

  //get the data from the passed folder
  data = getTigerFilesData(argv[1]);
  //connect these verticies
  verticies = createLinkedVerticies(data);
  //make sure the verticies are sorted
  qsort(&(verticies->content[0]), verticies->size, sizeof(Vertex*),(void*)VertexCompare);

  //using the data, determine the scale of the display based on the lat/long displayed
  determineLatLongScale();

  //set up verticies for shortest path implementation
  DUMMY_VERTEX = (Vertex*) malloc(sizeof(Vertex)); assert(DUMMY_VERTEX);
  startVertex = (Vertex*) malloc(sizeof(Vertex)); assert(startVertex);
  endVertex = (Vertex*) malloc(sizeof(Vertex)); assert(endVertex);

  Node* dummyNode = (Node*) malloc(sizeof(Node)); assert(dummyNode);
  dummyNode->longitude = DUMMY_ANGLE;
  dummyNode->latitude = DUMMY_ANGLE;
  DUMMY_VERTEX->start = dummyNode;

  startVertex = DUMMY_VERTEX;
  endVertex = DUMMY_VERTEX;

  //initialize OpenGL
  init();

  /* callback functions */
  glutDisplayFunc(display); 
  glutKeyboardFunc(keypress);
  glutMouseFunc(selectPoint);

  //create a menu to display when you right click
  glutCreateMenu(main_menu);
  glutAddMenuEntry("Show/Hide Roads", 1);
  glutAddMenuEntry("Show/Hide Hydrology", 2);
  glutAddMenuEntry("Show/Hide Railroads", 3);
  glutAddMenuEntry("Show/Hide Non Visible Features" , 6); 
  glutAddMenuEntry("Save currently displayed layers", 5);
  glutAddMenuEntry("Quit", 4);
  glutAttachMenu(GLUT_RIGHT_BUTTON);

  /* event handler */
  glutMainLoop();
  return 0;
}


/*Figures out the initial max and min lat and long locations to determine the
  initial view box */
void determineLatLongScale(){
  int currStateIndex, currCountyIndex;
  overallMaxLong = data->states[0].counties[0].maxLong;
  overallMaxLat = data->states[0].counties[0].maxLat;
  overallMinLong = data->states[0].counties[0].minLong;
  overallMinLat = data->states[0].counties[0].minLat;

  //goes through all the counties linearly (there shouldn't be that many) and finds the max and min lat and long
  for(currStateIndex = 0; currStateIndex < data->numStates; currStateIndex++){
    for(currCountyIndex=0; currCountyIndex < data->states[currStateIndex].numCounties; currCountyIndex++){

      County* currCounty;
      currCounty = &(data->states[currStateIndex].counties[currCountyIndex]);
      TIGER_DISPLAY_DEBUG{printf("looking at county %d out of %d\n", currCountyIndex, data->states[currStateIndex].numCounties); fflush(stdout);}

      if(currCounty->maxLong>overallMaxLong){
	TIGER_DISPLAY_DEBUG{printf("found bigger maxLong\n"); fflush(stdout);}
	overallMaxLong = currCounty->maxLong;
      }
      if(currCounty->maxLat>overallMaxLat){
	TIGER_DISPLAY_DEBUG{printf("found bigger maxLat\n"); fflush(stdout);}
	overallMaxLat = currCounty->maxLat;
      }
      if(currCounty->minLong<overallMinLong){
	TIGER_DISPLAY_DEBUG{printf("found smaller minLong\n"); fflush(stdout);}
	overallMinLong = currCounty->minLong;
      }
      if(currCounty->minLat<overallMinLat){
	TIGER_DISPLAY_DEBUG{printf("found smaller minLat\n"); fflush(stdout);}
	overallMinLat = currCounty->minLat;
      }
    }
  }

  TIGER_DISPLAY_DEBUG{printf("overallMaxLong=%d\t overallMinLong=%d\t overallMaxLat=%d\t overallMinLat=%d\n", (int)overallMaxLong, (int)overallMinLong, (int)overallMaxLat, (int)overallMinLat); fflush(stdout);}

  // these variables determine the width and height of the window
  int latDifference, longDifference;
  latDifference = fabs(overallMaxLat-overallMinLat);
  longDifference = fabs(overallMaxLong-overallMinLong);

  // we want to have a square view that can see everything, so we take the 
  //longer dimention and make that the ruler, or the width and the height

  if(latDifference > longDifference){
    ruler = (GLfloat)latDifference;
  }
  else{
    ruler = (GLfloat)longDifference;
  }

  TIGER_DISPLAY_DEBUG{printf("ruler=%f\n", (float)ruler); fflush(stdout);}
}





//called from the display function- draws the chain if it is valid.
void drawChain(Chain* chain){
  int drawChain = 0;
  switch(chain->CFCC[0]) {
  case 'A'://roads- draw black
    if(showRoad){
      if( (!showDetail && atoi(&chain->CFCC[2]) < 30) || 
	  (showDetail) ) {

	if(atoi(&chain->CFCC[2]) < 30){
	  //TIGER_DISPLAY_DEBUG{printf("rendering BIG road\t"); fflush(stdout);}
	}
	else{
	  //TIGER_DISPLAY_DEBUG{printf("rendering small road\t"); fflush(stdout);}
	}

	glColor3fv(gray);
	drawChain = 1;
      }
    }
    break;
  case 'B'://railroads- draw red
    if(showRail){
      if( (!showDetail && atoi(&chain->CFCC[2]) < 50) || 
	  (showDetail) ) {

	if(atoi(&chain->CFCC[2]) < 50){
	  //TIGER_DISPLAY_DEBUG{printf("BIG RAIL rendering\t"); fflush(stdout);}
	}
	else{
	  //TIGER_DISPLAY_DEBUG{printf("small RAIL rendering\t"); fflush(stdout);}
	}

	glColor3fv(red);
	drawChain = 1;
      }
    }
    break;
  case 'H'://Hydrography- draw blue
    if(showHydro){
      glColor3fv(blue);
      drawChain = 1;
    }
    break;
  case 'F': //Non visible features
    if(showNonVisible){
      glColor3fv(yellow);
      drawChain = 1;
    }
    break;
  }

  if(drawChain){
    glBegin(GL_LINE_STRIP);
    glVertex2i(chain->start.longitude, chain->start.latitude);
    
    if(showDetail){
      int i;
      //TIGER_DISPLAY_DEBUG{printf("numPoints =%d\n", chain->numPoints); fflush(stdout);}
      for(i=0; i<chain->numPoints; i++) { 
	glVertex2i(chain->points[i].longitude, chain->points[i].latitude);
	//TIGER_DISPLAY_DEBUG{printf("displaying shape point: %d, %d\t", chain->points[i].longitude, chain->points[i].latitude);}
	}
    }
    
    glVertex2i(chain->end.longitude, chain->end.latitude);
    glEnd();
  }
}



//displays everything
void display(void){

  glClear(GL_COLOR_BUFFER_BIT);

  //goes through data and draws all valid strings
  int currState;
  int currCounty;
  int currChain;
  for(currState = 0; currState < data->numStates; currState++){
    for(currCounty = 0; currCounty < data->states[currState].numCounties; currCounty++){
      //cuts out counties that cannot be seen
      if( ((data->states[currState].counties[currCounty].maxLong > 
	   (overallMinLong+zoomFactor)+eye[0]) &&
	  (data->states[currState].counties[currCounty].minLong <
	   ((overallMinLong+ruler)-zoomFactor)+eye[0]) ) &&
	  ( (data->states[currState].counties[currCounty].maxLat > 
	   (overallMinLat+zoomFactor)+eye[1]) &&
	  (data->states[currState].counties[currCounty].minLat < 
	   ((overallMinLat+ruler)-zoomFactor)+eye[1]) )
	 ) {
	//TIGER_DISPLAY_DEBUG{printf("rendering current county\n"); fflush(stdout);}
	for(currChain = 0; currChain < data->states[currState].counties[currCounty].numChains; currChain++){
	  drawChain(data->states[currState].counties[currCounty].chains[currChain]);
	}
      }
      else{
	//TIGER_DISPLAY_DEBUG{printf("skipping current county\n"); fflush(stdout);}
      }
    }
  }

  //hilights vertexes that have been clicked, if they have been clicked.
  TIGER_DISPLAY_DEBUG{printf("starting to highlight start and end vertecies, if valid\n"); fflush(stdout);}
  float i;
  //highlight start and end vertex, if valid
  if(startVertex != DUMMY_VERTEX){
    TIGER_DISPLAY_DEBUG{printf("start vertex valid: (%d, %d)\n", startVertex->start->longitude, startVertex->start->latitude); fflush(stdout);}
    glColor3fv(magenta);
    glBegin(GL_POLYGON);
    for(i=0; i < 2*3.1415; i+= (2*3.1415)/50){
      glVertex2i(startVertex->start->longitude + windowWidth/100*cos(i),
 		 startVertex->start->latitude + windowWidth/100*sin(i)); 
      //printf("got here: i = %f\t", i); fflush(stdout);
    }
    glEnd();
  }

  if(endVertex != DUMMY_VERTEX){
    TIGER_DISPLAY_DEBUG{printf("end vertex valid: (%d, %d)\n", endVertex->start->longitude, endVertex->start->latitude); fflush(stdout);}
    glColor3fv(magenta);
    glBegin(GL_POLYGON);
    for(i=0; i < 2*3.1415; i+= (2*3.1415)/50)
      glVertex2i(endVertex->start->longitude + windowWidth/100*cos(i),
 		 endVertex->start->latitude + windowWidth/100*sin(i));
    glEnd();

    //since endVetex can be drawn, we have a start and end vertex, 
    //so we should find the path between them.
    Path* shortPath = findShortPath(startVertex, endVertex, DUMMY_VERTEX);

    //if shortPath != NULL, we should draw it
    glColor3fv(magenta);
    glLineWidth(5.0);
    Vertex* temp;
    glBegin(GL_LINE_STRIP);
    int index;
    for(index=0; index<shortPath->verticies->size; index++){
      temp = (Vertex*)(shortPath->verticies->content[index]);
      glVertex2i(temp->start->longitude, temp->start->latitude);
    }
    glEnd();
    glLineWidth(1.0);
  }
  glFlush();		  
}


//initializes all the Open GL stuff
void init (void){
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  //figure out the initial bounds of the viewing screen
  viewLeftSide = overallMinLong;
  viewRightSide = overallMinLong+ruler;
  viewBottomSide = overallMinLat;
  viewTopSide = overallMinLat+ruler;

  //set the persecitve and location
  glOrtho(viewLeftSide, viewRightSide, viewBottomSide, viewTopSide, -1.0, 10.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(eye[0], eye[1], 10.0 , eye[0], eye[1], 0.0, 0.0, 1.0, 0.0);

  //figure out the window width- was going to be used to turn on and off detail- never quite finished.
  windowWidth = (overallMinLong+ruler)-overallMinLong;
  if(windowWidth < 2000000){showDetail = 1;}
  else{showDetail = 0;}
  TIGER_DISPLAY_DEBUG{printf("showDetail = %d\n", (int)showDetail); fflush(stdout);}
  //glPushMatrix();
  //glEnable(GL_DEPTH_TEST);
  //glDepthFunc(GL_LEQUAL);
}



//adjust the view based on user input
void changeLookingAt (void){

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  //figure out new bounds for the window based on the amount zoomed
  viewLeftSide = (overallMinLong+zoomFactor)+eye[0];
  viewRightSide = ((overallMinLong+ruler)-zoomFactor)+eye[0];
  viewBottomSide = (overallMinLat+zoomFactor)+eye[1];
  viewTopSide = ((overallMinLat+ruler)-zoomFactor)+eye[1];
  
  //set the window view
  glOrtho(viewLeftSide, viewRightSide, viewBottomSide, viewTopSide, -1.0, 10.0);

  //recalculate the window width, and start showing details if valid
  windowWidth = ((overallMinLong+ruler)-zoomFactor)-
    (overallMinLong+zoomFactor);
  if(windowWidth < 1500000){showDetail = 1;}
  else{showDetail = 0;}
  //TIGER_DISPLAY_DEBUG{printf("\n\n\n\nshowDetail = %d\n\n\n\n", showDetail); fflush(stdout);}

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

//write all visible data to a file
void outputData(){
  printf("creating output file\n");
  FILE* fout;
  //try to make the file
  fout = fopen("outputFile", "w+");
    if(!fout){
    printf("cannot create file\n");
    exit(1);
  }
  assert(fout);

  //go through all chains- if they are visible, write them to the file.
  int currState, currCounty, currChain;
  for(currState=0; currState< data->numStates; currState++){
    for(currCounty=0; currCounty < data->states[currState].numCounties; currCounty++){
      for(currChain=0; currChain < data->states[currState].counties[currCounty].numChains; currChain++){
	Chain* dataChain;
	dataChain = data->states[currState].counties[currCounty].chains[currChain];
	int saveChain;
	saveChain = 0;
	if(showRoad && dataChain->CFCC[0] == 'A'){saveChain = 1;}
	else if(showRail && dataChain->CFCC[0] == 'B'){saveChain = 1;}
	else if(showHydro && dataChain->CFCC[0] == 'H'){saveChain = 1;}
	else if(showNonVisible && dataChain->CFCC[0] == 'F'){saveChain = 1;}
	else if(showPhysical && dataChain->CFCC[0] == 'E'){saveChain = 1;}

	if(saveChain){
	  fprintf(fout, "%d\n%s\n%d %d\n%d %d\n\n", dataChain->TLID, dataChain->CFCC, dataChain->start.longitude, dataChain->start.latitude, dataChain->end.longitude, dataChain->end.latitude);
	}
      }
    }
  }
  printf("done creating file\n");
}


//handles key presses
void keypress(unsigned char key, int x, int y){
  switch(key){
    //quit
  case 'q':
    exit(0);
    break;
    //move right
  case 'l':
    eye[0] = eye[0]+(ruler/100);
    changeLookingAt();
    glutPostRedisplay();
    TIGER_DISPLAY_DEBUG{printf("moved right- window width now = %f\n", (float)(windowWidth)); fflush(stdout);}
    break;
    //move left
  case 'j':
    eye[0] = eye[0]-(ruler/100);
    changeLookingAt();
    glutPostRedisplay();
    TIGER_DISPLAY_DEBUG{printf("moved left- window width now = %f\n", (float)windowWidth); fflush(stdout);}
    break;
    //move down
  case 'k':
    eye[1] = eye[1]-(ruler/100);
    changeLookingAt();
    glutPostRedisplay();
    TIGER_DISPLAY_DEBUG{printf("moved down- window width now = %f\n", (float)windowWidth); fflush(stdout);}
    break;
    //move up
  case 'i':
    eye[1] = eye[1]+(ruler/100);
    changeLookingAt();
    glutPostRedisplay();
    TIGER_DISPLAY_DEBUG{printf("moved up- window width now = %f\n", (float)windowWidth); fflush(stdout);}
    break;
    //zoom out
  case 'u':
    zoomFactor -= (ruler/100);
    changeLookingAt();
    glutPostRedisplay();
    TIGER_DISPLAY_DEBUG{printf("zoomed out- window width now = %f\n", (float)windowWidth); fflush(stdout);}
    break;
    //zoom in
  case 'o':
    if(windowWidth-(2*(ruler/100)) > 1000){
      zoomFactor += (ruler/100);
      changeLookingAt();
      glutPostRedisplay();
      TIGER_DISPLAY_DEBUG{printf("zoomed in- window width now = %f\n", (float)windowWidth); fflush(stdout);}
    }
    break;
  }
}

//handles the menu
void main_menu(int value){
  switch(value){
  case 1: //roads
    showRoad = !showRoad;
    glutPostRedisplay();
    break;
  case 2: //hydrology
    showHydro = !showHydro;
    glutPostRedisplay();
    break;
  case 3: //rail
    showRail = !showRail;
    glutPostRedisplay();
    break;
  case 4: //quit
    exit(0);
    break;
  case 5: //save currently displayed layers
    outputData();
    break;
  case 6://non visible features
    showNonVisible = !showNonVisible;
    glutPostRedisplay();
    break;
  }
}

//finds the left side of the search area when looking for vertex in findNearestVertex
//basically a modified binary search that returns the index of the vertex at
//or just below the lower side of our bounding box
int findBoxMin(int key, int low, int high){

  TIGER_DISPLAY_DEBUG{printf("high=%d, low=%d\t", high, low); fflush(stdout);}

  //exact match not found- looking at neighbors find closest match
  if (high < low) {
    if(high <= 0) return 0;
    else return high-1;

  }

  //the index we care about, regardless if it is an exact match or not
  int mid = (low + high) /2;
  Vertex* midVertex = verticies->content[mid];
  TIGER_DISPLAY_DEBUG{printf("looking at vertex %d: %d, %d\t", mid, midVertex->start->longitude, midVertex->start->latitude); fflush(stdout);}


  if(key < midVertex->start->longitude){
    TIGER_DISPLAY_DEBUG{printf("key < mid\n"); fflush(stdout);}
    return findBoxMin(key, low, mid-1);
  }
  else if(key > midVertex->start->longitude){
    TIGER_DISPLAY_DEBUG{printf("key > mid\n"); fflush(stdout);}
    return findBoxMin(key, mid+1, high);
  }
  else{
    TIGER_DISPLAY_DEBUG{printf("key = mid\n"); fflush(stdout);}
    return mid;
  }
}

//when you click on the screen, it finds the nearest vertex in verticies to where you clicked
//It looks inside a small box.  If it does not find a valid vertex, it makes the box bigger.  Once it finds a vertex in the box, it compares the distance to that vertex with all the others in the box, and returns the vertex that is closest.
Vertex* findNearestVertex(Vertex* key){

  Vertex* result = NULL;
  int somethingInsideBox = 0;

  //as i grows, the box grows
  int i;
  for(i = 1; !somethingInsideBox; i++){

    //figures out the min long for the box- the long that was pressed minus i*20.  The max long is the long that was pressed + i*20
    int longMin = key->start->longitude - (i*20);
    TIGER_DISPLAY_DEBUG{printf("looking for a long between %d and %d\n", key->start->longitude - (i*20), key->start->longitude + (i*20)); fflush(stdout);}
    TIGER_DISPLAY_DEBUG{printf("looking for a lat between %d and %d\n", key->start->latitude - (i*20), key->start->latitude + (i*20)); fflush(stdout);}

    //finds the index of the first vertex that is inside the box.  Then, we linearly look through the array at all the vertecies until we reach one that has a long outside the box.
    int searchIndex = findBoxMin(longMin, 0, (int)verticies->size);
    Vertex* searchVertex = (Vertex*)verticies->content[searchIndex];
    TIGER_DISPLAY_DEBUG{printf("found a vertex: %d, %d\n", searchVertex->start->longitude, searchVertex->start->latitude); fflush(stdout);}
    float minDist = sqrt(pow(key->start->longitude - 
			     searchVertex->start->longitude, 2) + 
			 pow(key->start->latitude - 
			     searchVertex->start->latitude, 2));
    result = searchVertex;

    for(; searchIndex < verticies->size &&
	  searchVertex->start->longitude < key->start->longitude + (i*20); 
	searchIndex++){ 

      searchVertex = (Vertex*)verticies->content[searchIndex];
      TIGER_DISPLAY_DEBUG{printf("now looking at vertex: %d, %d\n", searchVertex->start->longitude, searchVertex->start->latitude); fflush(stdout);}
      //make sure the vertex we are looking at has a lat that is inside the box.
      //lat limits are calculated the same way as long limits
      if(searchVertex->start->latitude < 
	 key->start->latitude + (i*20) &&
	 searchVertex->start->latitude > 
	 key->start->latitude - (i*20)) {
	printf("got here\t"); fflush(stdout);
	//if the vertex is in the box, note that in somethingInsideBox, so we stop making the box bigger
	somethingInsideBox = 1;
	//compare distances, and change the stored values if the distance to this vertex is less than the distance to the stored vertex
	if(sqrt(pow(key->start->longitude - 
		    searchVertex->start->longitude, 2) + 
		pow(key->start->latitude - 
		    searchVertex->start->latitude, 2)) < minDist) {

	  minDist =  sqrt(pow(key->start->longitude - 
			      searchVertex->start->longitude, 2) + 
			  pow(key->start->latitude - 
			      searchVertex->start->latitude, 2));
	  result = searchVertex;
	}
      }
    }
 TIGER_DISPLAY_DEBUG{printf("distance=%f\n", minDist); fflush(stdout);}
  }
  TIGER_DISPLAY_DEBUG{printf("result = (%d, %d)\n\n", result->start->longitude, result->start->latitude); fflush(stdout);}
  return result;
}


//mouse listener that selects the nearest vertex in verticies to the mouse click
void selectPoint(int button, int state, int x, int y){

  if(button == GLUT_LEFT_BUTTON && state == GLUT_UP){

    TIGER_DISPLAY_DEBUG{printf("mouse pressed at %d, %d\n", x, y); fflush(stdout);}

    //determine the node that was selected
    TIGER_DISPLAY_DEBUG{printf("right=%f, left=%f, top=%f, bottom=%f\n", (float)viewRightSide, (float)viewLeftSide, (float)viewTopSide, (float)viewBottomSide);fflush(stdout);}

    int pressedX = floor(viewLeftSide+((x/500.0) * fabs(viewLeftSide-viewRightSide)) + 0.5);
    int pressedY = floor(viewTopSide-((y/500.0) * fabs(viewTopSide-viewBottomSide)) + 0.5);
    TIGER_DISPLAY_DEBUG{printf("lat/long pressed: %d, %d\n", pressedX, pressedY);
      fflush(stdout);}

    Vertex* nearestVertex = (Vertex*)malloc(sizeof(Vertex));
    Node* keyNode = (Node*)malloc(sizeof(Node));
    keyNode->longitude = pressedX;
    keyNode->latitude = pressedY;
    nearestVertex->start = keyNode;
    TIGER_DISPLAY_DEBUG{printf("starting findNearestVertex\n"); fflush(stdout);}
    nearestVertex = findNearestVertex(nearestVertex);

    //catch the first time through when startVertex == DUMMY_VERTEX
    if(startVertex == DUMMY_VERTEX){
      startVertex = nearestVertex;
      glutPostRedisplay();
    }
    //if endVertex != DUMMY_VERTEX, we need to set the startVertex 
    //and reset the end vertex
    else if(endVertex != DUMMY_VERTEX){
      startVertex = nearestVertex;
      endVertex = DUMMY_VERTEX;
      glutPostRedisplay();
    }
    //else, endVertex == DUMMY_VERTEX, so we need to fill it
    else{
      endVertex = nearestVertex;
      glutPostRedisplay();
    }
  }
}


