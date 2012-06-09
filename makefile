CC=g++
LIBS=-lGL -lGLU -lglut -lm -ljpeg

all: lunar_lander

lunar_lander: lunar_lander.o bibutil.o
	$(CC) -g -o lunar_lander lunar_lander.o bibutil.o $(LIBS)

bibutil.o: bibutil.cpp 
	$(CC) -g -c bibutil.cpp -w -Wall -pedantic

lunar_lander.o: lunar_lander.cpp 
	$(CC) -g -c lunar_lander.cpp -w -Wall -pedantic

clean:
	rm -f *.o
