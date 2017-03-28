CC=gcc
COPY=cp
MOVE=mv
RM=rm

CFLAGS=-c -Wall
LDFLAGS=

SOURCE_DIR=../src
BIN_DIR=../bin
TEST_DIR=../testdata

SOURCES=metaconverter.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=metaconverter

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@
	$(COPY) $(EXECUTABLE) $(TEST_DIR)/.
	$(MOVE) $(EXECUTABLE) $(BIN_DIR)/.
.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) *o $(BIN_DIR)/$(EXECUTABLE)
	$(RM) $(TEST_DIR)/csvs/*
	$(RM) $(TEST_DIR)/xmls/*

