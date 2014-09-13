CC=gcc
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=httpserver.c
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=httpserver

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(EXECUTABLE) *.*~ *~
