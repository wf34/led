

C=g++
CC=g++
CFLAGS=-c -Wall -I/usr/include
LDFLAGS=-lncurses -lpthread -levent
SOURCES=main.cpp model.cpp session.cpp view.cpp
OBJECTS=$(SOURCES:.cpp=.o)
	EXECUTABLE=serv

all: $(SOURCES) $(EXECUTABLE)
		
$(EXECUTABLE): $(OBJECTS)
		$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
		$(CC) $(CFLAGS) $< -o $@



