CC=clang++
CFLAGS=-c --std=c++11 -g -Wall -iquote include/
ifeq ($(OS),Windows_NT)
	LDFLAGS=-lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -lcurl -lSDL2_ttf -pthread

else
	LDFLAGS=-lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lcurl -pthread

endif
SDIR=src
SAUCES=btReader.cpp button.cpp content.cpp database.cpp getImage.cpp httpd.cpp image.cpp main.cpp novelList.cpp object.cpp textbox.cpp wikiParser.cpp pugixml.cpp novelDetails.cpp events.cpp keyMap.cpp logger.cpp menu.cpp
SOURCES=$(patsubst %,$(SDIR)/%,$(SAUCES))
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
