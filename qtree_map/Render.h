#ifndef __Render_h
#define __Render_h

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "Map.h"
#include "Reader.h"

//main function.  Should take data path and max and min bounds as arguments
int main(int argc, char** argv);

//initializes the OpenGL stuff
void init(void);

//displays the data
void display(void);

//renders the passed disk level
void renderDiskLevel(int level);

//renders the passed mem level
void renderMemLevel(int level);

//draw an RT1
void drawRT1(RT1 rt1);

//takes care of setting the global level
void setLevel();

//handles key presses
void keypress(unsigned char key, int x, int y);

#endif
