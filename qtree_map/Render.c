#include "Render.h"

#define RENDER_DEBUG if(0)

GLfloat red[3] = {1.0, 0.0, 0.0};
GLfloat green[3] = {0.0, 1.0, 0.0};
GLfloat blue[3] = {0.0, 0.0, 1.0};
GLfloat black[3] = {0.0, 0.0, 0.0};
GLfloat white[3] = {1.0, 1.0, 1.0};
GLfloat gray[3] = {0.5, 0.5, 0.5};
GLfloat yellow[3] = {1.0, 1.0, 0.0};
GLfloat magenta[3] = {1.0, 0.0, 1.0};
GLfloat cyan[3] = {0.0, 1.0, 1.0};

//center[0]->x, center[1]->y
GLdouble center[2];

//dimension[0]->width, dimension[1]->height
GLdouble dimension[2];

Map* map;

int level;


int main(int argc, char** argv){

  //if incorrect information is passed, yell at the user
  if(argc > 2){
    printf("incorrect usage.  Correct usage is 'qtree_map <data path>' to make new files \nOR\n 'qtree_map' to leave the tiles as they are\n");
    //formorly: 'qtree_map <data path> <min longitude> <min latitude> <max longitude> <max latitude> <cell width> <cell height>'
    exit(1);
  }

  RENDER_DEBUG{
    //printf("geting filemap\n"); fflush(stdout);
  }
  //get the data from the data path
  if(argc == 1)
    map = Reader_read();
  if(argc == 2)
    map = Reader_readAndCreate(argv[1]);

  RENDER_DEBUG{
    //printf("finished getting filemap\n"); fflush(stdout);
  }

  //initialize center with center values
  center[0] = (Map_getMaxLon(*map) + Map_getMinLon(*map)) / 2.0;
  center[1] = (Map_getMaxLat(*map) + Map_getMinLat(*map)) / 2.0;

  //initialize dimension with differences
  dimension[0] = (Map_getMaxLon(*map) - Map_getMinLon(*map)) / 2.0;
  dimension[1] = (Map_getMaxLat(*map) - Map_getMinLat(*map)) / 2.0;

  // open a window
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);  

  float lonDiff = Map_getMaxLon(*map) - Map_getMinLon(*map);
  float latDiff = Map_getMaxLat(*map) - Map_getMinLat(*map);
  //making the window size match the ratio of the two dimensions
  if(lonDiff > latDiff)
    glutInitWindowSize(900, 900*(latDiff/lonDiff));
  else 
    glutInitWindowSize(900*(lonDiff/latDiff), 900);


  glutInitWindowPosition(50,50);
  glutCreateWindow(argv[0]);
  init();

  /* OpenGL init */
  glClearColor(1.0, 1.0, 1.0, 0);   /* set background color white*/

  /* callback functions */
  glutDisplayFunc(display); 
  glutKeyboardFunc(keypress);

  /* event handler */
  glutMainLoop();
  return 0;
}

//initializes the OpenGL stuff
void init(void){
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  
  //printf("left = %f, right = %f, bottom = %f, top = %f\n", 
  //	 center[0] - dimension[0], center[0] + dimension[0], 
  //	 center[1] - dimension[1], center[1] + dimension[1]); fflush(stdout);
  //set the persecitve and location
  glOrtho(center[0] - dimension[0], center[0] + dimension[0], 
	  center[1] - dimension[1], center[1] + dimension[1], 
	  0.0, 2.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

//displays the data
void display(void){
  
  //printf("DISPLAY: left = %f, right = %f, bottom = %f, top = %f\n", 
  //	 center[0] - dimension[0], center[0] + dimension[0], 
  //	 center[1] - dimension[1], center[1] + dimension[1]); fflush(stdout);
  //printf("center = %f %f\tdimension = %f %f\n", center[0], center[1], dimension[0], dimension[1]); fflush(stdout);

  glClear(GL_COLOR_BUFFER_BIT);
  //move and zoom, if need be
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();  
  //set the persecitve and location
  glOrtho(center[0] - dimension[0], center[0] + dimension[0], 
	  center[1] - dimension[1], center[1] + dimension[1], 
	  0.0, 2.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  //start printing RT1's
  //printf("displaying\n");
  if(level == 5) {
    renderDiskLevel(level);
  }
  else{
    renderMemLevel(level);
  }
  glFlush();
  //printf("done dispaying\n\n");
}

//renders the passed disk level
void renderDiskLevel(int level){
  //printf("rendering disk level %d\n", level); fflush(stdout);
  int i, j;
  int rt1Counter = 0;
  RT1* RT1array;
  int numRT1;
  for(i = 0; i < Map_getFileCol(*map); i++){
    for(j = 0; j < Map_getFileRow(*map); j++){

      FileTile* tile = Map_getFileTile(*map, i, j);

      if( (center[0] - dimension[0] > FileTile_getMaxLon(*tile)) ||
	  (center[1] - dimension[1] > FileTile_getMaxLat(*tile)) ||
	  (center[0] + dimension[0] < FileTile_getMinLon(*tile)) ||
	  (center[1] + dimension[1] < FileTile_getMinLat(*tile)) ){
	//printf("pruning tile with left %d, right %d, bottom %d, top %d\n", FileTile_getMinLon(*tile), FileTile_getMaxLon(*tile), FileTile_getMinLat(*tile), FileTile_getMaxLat(*tile)); fflush(stdout);	
	continue;
      }
      
      RT1array = Map_getFileTileRT1(*map, i, j);
      numRT1 = FileTile_getNumRT1(*tile);
      
      assert(RT1array); 
      
      int k;
      //printf("\nprinting %d RT1 in level %d tile (%d, %d)\n", numRT1, level, i, j);
      for(k = 0; k < numRT1; k++){
	drawRT1(RT1array[k]);
	rt1Counter++;
	if((rt1Counter % 10000) == 1){
	  //printf("."); fflush(stdout);
	}
      }
      free(RT1array);
    }
  }
  //printf("done RenderDiskLevel()\n");
}

//renders the passed mem level
void renderMemLevel(int level){
  //printf("rendering mem level %d", level); fflush(stdout);
  int i, j;
  int rt1Counter = 0;
  RT1* RT1array;
  int numRT1;
  for(i = 0; i < map->memTileCol[level] ; i++){
    for(j = 0; j < map->memTileRow[level]; j++){

      MemTile* tile = Map_getMemTile(*map, level, i, j);
      
      if( (center[0] - dimension[0] > MemTile_maxLon(*tile)) ||
	  (center[1] - dimension[1] > MemTile_maxLat(*tile)) ||
	  (center[0] + dimension[0] < MemTile_minLon(*tile)) ||
	  (center[1] + dimension[1] < MemTile_minLat(*tile)) ){
	//printf("pruning tile with left %d, right %d, bottom %d, top %d\n", MemTile_minLon(*tile), MemTile_maxLon(*tile), MemTile_minLat(*tile), MemTile_maxLat(*tile)); fflush(stdout);	
	continue;
      }
      
      RT1array = Map_getMemTileRT1(*map, level, i, j);
      numRT1 = MemTile_dataSize(*Map_getMemTile(*map, level, i, j));
      
      assert(RT1array); 
      
      int k;
      //printf("\nprinting %d RT1 in level %d\n", numRT1, level);
      for(k = 0; k < numRT1; k++){
	drawRT1(RT1array[k]);
	rt1Counter++;
	if((rt1Counter % 10000) == 1){
	  //printf("."); fflush(stdout);
	}
      }
    }
  }
  //printf("done renderMemLevel()\n");
}



//draw an RT1
void drawRT1(RT1 rt1){
  int drawChain = 0;
  switch(RT1_getType(&rt1)[0]) {
  case 'A'://roads- draw black
    glColor3fv(red);
    drawChain = 1;
    break;
  case 'B': //rail - draw red
    drawChain = 1;
    glColor3fv(gray);
    break;
  case 'F': //non-visible- draw yellow
    drawChain = 1;
    glColor3fv(black);
    break;
  case 'H': //water- blue
    drawChain = 1;
    glColor3fv(blue);
    break;
  }
  
  RENDER_DEBUG{
    //printf("drawing %d, %d to %d, %d\n", Node_getLon(RT1_getStart(rt1)), Node_getLat(RT1_getStart(rt1)), Node_getLon(RT1_getEnd(rt1)), Node_getLat(RT1_getEnd(rt1))); fflush(stdout);
  }

  if(drawChain){
    glBegin(GL_LINES);
    glVertex2i(Node_getLon(RT1_getStart(rt1)), Node_getLat(RT1_getStart(rt1)));
    glVertex2i(Node_getLon(RT1_getEnd(rt1)), Node_getLat(RT1_getEnd(rt1)));
    glEnd();
  }
}

//takes care of setting the global level
void setLevel(){
  if(dimension[1]*2 >= Map_getCellWidth(*map) * pow(2, 4)) {level = 0;}
  else if (dimension[1]*2 >= Map_getCellWidth(*map) * pow(2, 3)) {level = 1;} 
  else if (dimension[1]*2 >= Map_getCellWidth(*map) * pow(2, 2)) {level = 2;} 
  else if (dimension[1]*2 >= Map_getCellWidth(*map) * pow(2, 1)) {level = 3;} 
  else if (dimension[1]*2 >= Map_getCellWidth(*map) * pow(2, 0)) {level = 4;} 
  else {level = 5;} 
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
    center[0] += dimension[0]/10;
    setLevel();
    glutPostRedisplay();
    break;
    //move left
  case 'j':
    center[0] -= dimension[0]/10;
    setLevel();
    glutPostRedisplay();
    break;
    //move down
  case 'k':
    center[1] -= dimension[1]/10;
    setLevel();
    glutPostRedisplay();
    break;
    //move up
  case 'i':
    center[1] += dimension[1]/10;
    setLevel();
    glutPostRedisplay();
    break;
    //zoom out
  case 'u':
    dimension[0] *= 1.25;
    dimension[1] *= 1.25;
    setLevel();
    glutPostRedisplay();
    break;
    //zoom in
  case 'o':
    dimension[0] *= 0.8;
    dimension[1] *= 0.8;
    setLevel();
    glutPostRedisplay();
    break;
  }
}
