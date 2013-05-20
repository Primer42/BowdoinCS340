#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "BW_DArray.h"

#define DARRAY_DEBUG if(0)

//Construct new DArray
DArray* newDArray(long initSize, long incSize) {
  DArray* new = (DArray*) malloc(sizeof(DArray)); assert(new);

  new->content = (void**) malloc(initSize*sizeof(void*)); assert(new->content);
  new->size = 0;
  new->maxSize = initSize;
  new->inc = incSize;

  return new;
} 

//Clone the passed DArray and return the new, copy
DArray* DArrayClone(DArray* array) {
  assert(array);

  DArray* new = (DArray*) malloc(sizeof(array)); assert(new);
  new->content = (void**) malloc(sizeof(array->content)); assert(new->content);
  
  int i = 0;
  for(;i < array->size; i++) {
    new->content[i] = array->content[i];
  }
  new->size = array->size;
  new->maxSize = array->maxSize;
  new->inc = array->inc;

  return new;
}

//adds element to DArray
void DArrayAdd(DArray* array, void* element) {
  assert(array); assert(element);
  DARRAY_DEBUG{printf("array has size %ld of %ld\n", array->size, array->maxSize); fflush(stdout);}
  if(array->size == array->maxSize-1){
    DARRAY_DEBUG{printf("growing array\n\n"); fflush(stdout);}
    DArrayGrow(array);
  }
  array->content[array->size] = element;
  array->size++;
}

//get element from Darray
void* DArrayGet(DArray* array, long position) {
  assert(array);
  return array->content[position];
}

//grow new DArray
void DArrayGrow(DArray* array) {
  assert(array);
  array->content =
    (void**)realloc(array->content, (array->maxSize+array->inc)*sizeof(void*));
  array->maxSize = array->maxSize + array->inc;
}
