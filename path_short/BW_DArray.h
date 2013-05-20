#ifndef _DArray_h
#define _DArray_h

typedef struct _DArray {
  void** content;
  long size;
  long maxSize;
  long inc;
} DArray;

//Construct new DArray
DArray* newDArray(long initSize, long incSize);

//Clone the passed DArray and return the new, copy
DArray* DArrayClone(DArray* array);

//adds element to DArray
void DArrayAdd(DArray* array, void* element);

//get element from Darray
void* DArrayGet(DArray* array, long position);

//grow new DArray
void DArrayGrow(DArray* array);

#endif
