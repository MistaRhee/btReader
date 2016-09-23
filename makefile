CC=clang++
CFLAGS=-c -O3 --std=c++14 -g -Wall -iquote include/ -Werror
LDFLAGS=-Llib -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lcurl -pthread -lcrypto
SDIR=src/
SOURCES=$(wildcard ./$(SDIR)*.cpp)
OBJECTS=$(SOURCES:.cpp=.o)

ifeq ($(OS),Windows_NT)
	EXECUTABLE=reader.exe
else
	EXECUTABLE=reader
endif

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
		$(CC) $(CFLAGS) $< -o $@

.PHONY: clean

clean: 
	rm -f src/*.o
	rm -f $(OBJECTS)

hardClean:
	rm -f src/*.o
	rm -f $(OBJECTS)
	rm -f $(EXECUTABLE)

testClean:
	rm -rf data/chaps/
	rm -rf data/novels/
	rm -rf data/novels.db
	rm -rf data/temp/
