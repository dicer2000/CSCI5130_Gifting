# Improved Makefile by Brett Huffman v1.1
# (c)2021 Brett Huffman
# To use, just change the next line to the name of
# the application

# App 1 - builds the gifting program
appname1 := gifting
srcfiles := $(shell find . -name "gift*.cpp")
objects1  := $(patsubst %.cpp, %.o, $(srcfiles))
CXXFLAGS := -g -std=c++11 -Wall -pedantic

all: $(appname1)

$(appname1): $(objects1)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(appname1) $(objects1) $(LDLIBS)

# App 2 - builds the bin_adder program
#appname2 := bin_adder
#srcfiles := $(shell find . -name "bin_adder*.cpp")
#objects2  := $(patsubst %.cpp, %.o, $(srcfiles))

#all: $(appname2)

#$(appname2): $(objects2)
#	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(appname2) $(objects2) $(LDLIBS)

clean:
	rm -f $(objects1)
	rm -f $(appname1)
#	rm -f $(objects2)
#	rm -f $(appname2)