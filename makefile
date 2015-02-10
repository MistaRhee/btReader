CC = g++
CFLAGS = --std=c++11 -g -Wall -iquote include/
ifeq ($(OS),Windows_NT)
	LDFLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -pthread

else
	LDFLAGS = -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lcurl -pthread

endif
SAUCES = src/*.cpp 
OBJECTS = $(SOURCES:.cpp=.o)

ifeq ($(OS),Windows_NT)
	EXECUTABLE = reader.exe
else
	EXECUTABLE = reader
endif

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
		$(CC) $(CFLAGS) $< -o $@
