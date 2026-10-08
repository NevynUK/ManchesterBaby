appname := ssem_main

CC=gcc
CXX=g++
RM=rm
CXXFLAGS=--std=c++17 -Wall -Wextra -pedantic -g

srcfiles := $(shell find . -name "*.cxx")
srcfiles += $(shell find ../NuttX//Core -name "*.cxx")

INCLUDES := -I../NuttX/include

ifdef UNIT_TESTS
DEFINES += -DUNIT_TESTS
srcfiles += $(shell find ../NuttX/UnitTests -name "*.cxx")
endif

objects  := $(patsubst %.cxx, %.o, $(srcfiles))

all: $(appname)

$(appname): $(objects)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEFINES) -o $(appname) $(objects)

%.o: %.cxx
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEFINES) -c $< -o $*.o

clean:
	$(RM) -f $(objects) $(appname)
