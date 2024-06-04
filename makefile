CXX     := g++
OUTPUT  := GeometryWars

SFML_DIR := ./dependencies/sfml/2.6.1

# compiler and linker flags
CXX_FLAGS := -O3 -std=c++17
INCLUDES  := -I./src -I$(SFML_DIR)/include
LDFLAGS   := -O3 -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -L$(SFML_DIR)/lib

# the source files for the ecs game engine
SRC_FILES := $(wildcard src/*.cpp)
OBJ_FILES := $(patsubst src/%.cpp, objects/%.o, $(SRC_FILES))

# all of these targets will be made if you just type make
all: $(OUTPUT)

# create bin and objects directories if they don't exist
$(OUTPUT): | bin objects

bin:
	mkdir -p bin

objects:
	mkdir -p objects

# define the main executable requirements / command
$(OUTPUT): $(OBJ_FILES) Makefile
	$(CXX) $(OBJ_FILES) $(LDFLAGS) -o ./bin/$@

# specifies how the object files are compiled from cpp files
objects/%.o: src/%.cpp | objects
	$(CXX) -c $(CXX_FLAGS) $(INCLUDES) $< -o $@

# typing 'make clean' will remove all intermediate build files
clean:
	rm -r bin objects

# typing 'make run' will compile and run the program
run: $(OUTPUT)
	cd bin && ./GeometryWars && cd ..
