# Assigned to: Timothy Sparks, Tony McFarlan, Haddon Stauffer, Zunain Nazir (shared project setup).
CXX = g++
CPPFLAGS += -Iinclude
CXXFLAGS += -std=c++17 -Wall -Wextra -Wpedantic -MMD -MP

SOURCES := $(wildcard src/*.cpp)
OBJECTS := $(patsubst src/%.cpp,build/%.o,$(SOURCES))
TARGET := build/mips-simulator

.PHONY: all clean
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

build/%.o: src/%.cpp | build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

build:
	mkdir -p $@

clean:
	rm -rf build

-include $(OBJECTS:.o=.d)
