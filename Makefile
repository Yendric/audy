CC=g++
CFLAGS=-g -Wall -municode -mwindows -lole32 -O3
SRC=src
OBJ=obj
SRCS=$(wildcard $(SRC)/*.cpp)
OBJS=$(patsubst $(SRC)/%.cpp, $(OBJ)/%.o, $(SRCS))

BINDIR=bin
BIN=$(BINDIR)/audy

all: resource.res $(BIN)

resource.res: $(SRC)/resource.rc
	windres $(SRC)/resource.rc -O coff -o  $(OBJ)/resource.res

$(BIN): $(OBJS)
	$(CC) $(OBJS) -o $@ $(OBJ)/resource.res $(CFLAGS)

$(OBJ)/%.o: $(SRC)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) -r $(BINDIR)/* $(OBJ)/*

