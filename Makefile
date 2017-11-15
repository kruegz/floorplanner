CC=g++
CFLAGS=-c -Wall -std=c++11 -O3
LDFLAGS=

SOURCES=main.cpp floorplan.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=floorplanner

# Default make option
all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o floorplanner
