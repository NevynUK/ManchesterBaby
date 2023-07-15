appname := ssem_main

CC=gcc
CXX=g++
RM=rm -f
CXXFLAGS=--std=c++17 -Wall -Wextra -pedantic -g

srcfiles := $(shell find . -name "*.cxx")
srcfiles += $(shell find ../NuttX//Core -name "*.cxx")
srcfiles += $(shell find ../NuttX/UnitTests -name "*.cxx")

objects  := $(patsubst %.cxx, %.o, $(srcfiles))

INCLUDES := -I../NuttX/include

all: $(appname)

$(appname): $(objects)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEFINES) -o $(appname) $(objects)

%.o: %.cxx
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEFINES) -c $< -o $*.o

clean:
	$(RM) -f $(objects) $(appname)
