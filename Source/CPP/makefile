appname := ssem_main

CC=gcc
CXX=g++
RM=rm -f
CXXFLAGS=--std=c++17

srcfiles := $(shell find . -name "*.cxx")
objects  := $(patsubst %.cxx, %.o, $(srcfiles))

all: $(appname)

$(appname): $(objects)
	$(CXX) $(CXXFLAGS) -o $(appname) $(objects)

%.o: %.cxx
	$(CXX) $(CXXFLAGS) -c $< -o $*.o

clean:
	$(RM) -f $(objects) $(appname)
