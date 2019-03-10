TARGET = obwody
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -O2

.PHONY: default all clean

default: $(TARGET)
all: clean default

OBJECTS = obwody.cc

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -Wall $(LIBS) -o $@

clean:
	-rm -f *.o
	-rm -f $(TARGET)

debug: CXXFLAGS += -DDEBUG -Wshadow -g -O0
debug: clean default
