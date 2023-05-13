CXX=g++
SRC=main.cpp
OBJ=$(SRC:.cpp=.o)
TARGET=main

all: $(TARGET) 
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

%.o: %.cpp %.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<


clean:
	rm -f $(OBJ) $(TARGET) $(DEP)