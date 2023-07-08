appname := ssem_main

CC=gcc
CXX=g++
RM=rm -f
CXXFLAGS=--std=c++17

srcfiles := $(shell find . -name "*.cxx")
srcfiles += $(shell find ../Core -name "*.cxx")
srcfiles += $(shell find ../UnitTests -name "*.cxx")

objects  := $(patsubst %.cxx, %.o, $(srcfiles))

INCLUDES := -I../Core -I../UnitTests -I../include

all: $(appname)

$(appname): $(objects)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(appname) $(objects)

%.o: %.cxx
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $*.o

clean:
	$(RM) -f $(objects) $(appname)
