CC=x86_64-w64-mingw32-gcc
CFLAGS=-g -Wall -municode -mwindows -lole32 -lshcore -lwinmm -O3 
SRC=src
OBJ=obj
SRCS=$(wildcard $(SRC)/*.c)
OBJS=$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))

BINDIR=bin
BIN=$(BINDIR)/audy

all: resource.res $(BIN)

resource.res: $(SRC)/resource.rc
	mkdir -p $(OBJ)
	x86_64-w64-mingw32-windres $(SRC)/resource.rc -O coff -o $(OBJ)/resource.o

$(BIN):
	mkdir -p $(BINDIR)
	$(CC) $(SRCS) $(OBJ)/resource.o -o $@ $(CFLAGS)

clean:
	rm -rf $(BINDIR) $(OBJ)

.PHONY: run
run: all
	$(BIN).exe