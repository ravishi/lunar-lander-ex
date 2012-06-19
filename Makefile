CC=g++
CPPFLAGS=-g -c -w -Wall -pedantic -I./box2d/include
LDFLAGS=-g -L./box2d/lib
LIBS=-lGL -lGLU -lglut -lm -ljpeg -lBox2D

all: lunar_lander

lunar_lander: lunar_lander.o bibutil.o
	$(CC) $(LDFLAGS) -o lunar_lander lunar_lander.o bibutil.o $(LIBS)

bibutil.o: bibutil.cpp 
	$(CC) $(CPPFLAGS) bibutil.cpp

lunar_lander.o: lunar_lander.cpp 
	$(CC) $(CPPFLAGS) lunar_lander.cpp

clean:
	rm -f *.o
