# Makefile

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

# Directories
SRC_DIR = ./src
BUILD_DIR = ./build
INCLUDE_DIR = /opt/homebrew/Cellar/sfml/2.6.1/include
LIB_DIR = /opt/homebrew/Cellar/sfml/2.6.1/lib

# Libraries
LIBS = -lsfml-graphics -lsfml-window -lsfml-system

# Target executable
TARGET = $(BUILD_DIR)/main.out

# Source files and object files
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC_FILES))

# Default rule
all: $(TARGET)

# Build rule
$(TARGET): $(OBJ_FILES)
	$(CXX) $(OBJ_FILES) $(CXXFLAGS) -o $(TARGET) -L$(LIB_DIR) $(LIBS)

# Rule to create object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Clean rule
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
