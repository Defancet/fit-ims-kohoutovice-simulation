.PHONY = all clean
CXX = g++
TARGET = main
SIMLIB = -lsimlib -lm

all: $(TARGET)

$(TARGET): $(TARGET).cpp
	$(CXX) $(CXXFLAGS) $< $(LIBS) $(SIMLIB) -o $@

clean:
	rm -rf *.o $(TARGET)