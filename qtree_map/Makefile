CC = gcc -O3 -Wall -DNDEBUG

LDFLAGS = $(LDLIBS) $(GLDLIBS) -lm
GLDLIBS = -framework AGL -framework OpenGL -framework GLUT -framework Foundation

O_FILES = Render.o Reader.o Map.o FileTile.o MemTile.o RT1.o Node.o

PROGS = qtree_map

default: $(PROGS)

qtree_map: $(O_FILES)
	$(CC) $(LDFLAGS) $(O_FILES) -o $@

Render.o: Render.c Render.h Reader.o Map.o FileTile.o MemTile.o
	$(CC) -c $< -o $@

Reader.o: Reader.c Reader.h Map.o MemTile.o
	$(CC) -c $< -o $@

Map.o: Map.c Map.h RT1.o FileTile.o MemTile.o
	$(CC) -c $< -o $@

FileTile.o: FileTile.c FileTile.h RT1.o
	$(CC) -c $< -o $@

MemTile.o: MemTile.c MemTile.h RT1.o
	$(CC) -c $< -o $@

RT1.o: RT1.c RT1.h Node.o
	$(CC) -c $< -o $@

Node.o: Node.c Node.h
	$(CC) -c $< -o $@


clean:
	$(RM) *.o qtree_map