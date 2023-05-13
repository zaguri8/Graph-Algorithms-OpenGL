CXX=g++
GLM_PATH=$(shell brew --prefix glm)
GLFW_PATH=$(shell brew --prefix glfw)
CXXFLAGS=-std=c++11 -I$(GLM_PATH)/include -I$(GLFW_PATH)/include -I/usr/include/graphics
LDFLAGS=-lgraphics -framework OpenGL
LDFLAGS+= -L$(shell brew --prefix glfw)/lib -lglfw
SRC=main.cpp
HEADERS=$(SRC:.cpp=.hpp)
OBJ=$(SRC:.cpp=.o)
TARGET=main

all: $(TARGET) 
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

%.o: %.cpp %.hpp graph.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

-include $(DEP)

clean:
	rm -f $(OBJ) $(TARGET) $(DEP)