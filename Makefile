CC = gcc
CXX = g++
CXXFLAGS = -O3
CFLAGS = -O3 -lm -pthread -fopenmp 

# hw3: CFLAGS +=  -pthread -fopenmp 
CXXFLAGS = $(CFLAGS)
TARGETS = hw3 #gen_input

.PHONY: all
all: $(TARGETS)

.PHONY: clean
clean:
	rm -f $(TARGETS) $(TARGETS:=.o)