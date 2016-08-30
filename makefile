CC=clang++
CFLAGS=-c -O3 --std=c++14 -g -Wall -iquote include/ 
ifeq ($(OS),Windows_NT)
	LDFLAGS=-Llib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -lcurl -lSDL2_ttf -pthread -lmistarheeCrypto

else
	LDFLAGS=-Llib -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lcurl -pthread -lmistarheeCrypto

endif
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
