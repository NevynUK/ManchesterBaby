appname := ssem_main

CC=gcc
CXX=g++
RM=rm -f

# Build type: use `make BUILD=debug` or `make BUILD=release` (default: debug)
BUILD ?= debug

ifeq ($(BUILD), release)
    BUILDFLAGS := -O2 -DNDEBUG
else
    BUILDFLAGS := -g -O0 -DDEBUG
endif

CXXFLAGS=--std=c++20 -Wall -Wextra -pedantic $(BUILDFLAGS)

srcfiles := $(shell find . -name "*.cpp")
srcfiles += $(shell find ../../components/ALU -name "*.cpp")

objects  := $(patsubst %.cpp, %.o, $(srcfiles))

INCLUDES := -I../../components/ALU

all: $(appname)

$(appname): $(objects)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEFINES) -o $(appname) $(objects)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEFINES) -c $< -o $*.o

clean:
	$(RM) -f $(objects) $(appname)
