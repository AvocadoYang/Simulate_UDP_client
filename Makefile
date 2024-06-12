# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = $(shell pkg-config --cflags --libs opencv4)

# Targets
TARGETS = Thermal.out AI.out

# Source files
SOURCES = Thermal.cpp AI.cpp

# Default target
all: $(TARGETS)

# Rule to build Thermal.out
Thermal.out: Thermal.cpp
	$(CXX) -o $@ $< $(CXXFLAGS)

# Rule to build AI.out
AI.out: AI.cpp
	$(CXX) -o $@ $< $(CXXFLAGS)

# Clean up build files
clean:
	rm -f $(TARGETS)

.PHONY: all clean

opencv: 
	sudo apt-get update | sudo apt-get install libopencv-dev

run:
	gnome-terminal -- bash -c ./AI.out | gnome-terminal -- bash -c ./Thermal.out