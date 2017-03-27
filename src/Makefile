CC=gcc
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=metaconverter.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=metaconverter

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *o $(EXECUTABLE)

