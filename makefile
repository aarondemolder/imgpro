CC=clang
CFLAGS=
LDFLAGS=-l SDL2 -l SDL2_image -l m
SOURCES=imgpro.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=IMGPRO

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
