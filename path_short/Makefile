
CC = gcc

LDLIBS =
GLDLIBS = -framework AGL -framework OpenGL -framework GLUT -framework Foundation
LDFLAGS  = $(LDLIBS) $(GLDLIBS) -lm

CFLAGS = -g -Wall

default: W_DisplayTigerData

W_DisplayTigerData: W_DisplayTigerData.o W_GatherTigerData.o BW_LinkedVerticies.o BW_DArray.o BW_Path.o BW_FindPath.o pqheap.o
	$(CC) $(CFLAGS) $(LDFLAGS) W_DisplayTigerData.o W_GatherTigerData.o BW_LinkedVerticies.o BW_DArray.o BW_Path.o BW_FindPath.o pqheap.o -o $@


W_DisplayTigerData.o: W_DisplayTigerData.c
	$(CC) $(CFLAGS) -c $< -o $@

W_GatherTigerData.o: W_GatherTigerData.c W_GatherTigerData.h
	$(CC) $(CFLAGS) -c $< -o $@

BW_LinkedVerticies.o: BW_LinkedVerticies.c BW_LinkedVerticies.h
	$(CC) $(CFLAGS) -c $< -o $@

BW_Path.o: BW_Path.c BW_Path.h
	$(CC) $(CFLAGS) -c $< -o $@

BW_FindPath.o: BW_FindPath.c BW_FindPath.h
	$(CC) $(CFLAGS) -c $< -o $@

BW_DArray.o: BW_DArray.c BW_DArray.h
	$(CC) $(CFLAGS) -c $< -o $@

pqheap.o: pqheap.c pqheap.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:	
	$(RM) W_DisplayTigerData W_DisplayTigerData.o W_GatherTigerData.o BW_LinkedVerticies.o BW_Path.o BW_FindPath.o BW_DArray.o pqheap.o