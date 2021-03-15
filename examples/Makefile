# It may be a good idea to set a `FUTAR_HOME` global environment variable.
# That way this Makefile would work in any directory.
FUTAR_HOME ?= $(PWD)/../

# Try to pick out whether `CC` exists (for NERSC systems)
# If not, try to use mpic++.
ifeq (, $(shell which CC))
	CXX = g++
else
	CXX = CC
endif

SOURCES += $(wildcard *.cpp)
TARGETS := $(patsubst %.cpp, %, $(SOURCES))

CXXFLAGS = -std=gnu++17 -O3 -lpthread -I$(FUTAR_HOME)

all: $(TARGETS)

%: %.cpp
	$(CXX) -o $@ $^ $(CXXFLAGS)

clean:
	rm -fv $(TARGETS)

