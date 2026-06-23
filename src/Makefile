CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET = mux_optimizer
SOURCES = main.cpp gates.cpp multiplexer.cpp optimizer.cpp
OBJECTS = $(SOURCES:.cpp=.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "✓ Build complete: ./$(TARGET)"

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET) optimization_results.csv
	@echo "✓ Cleaned build artifacts"