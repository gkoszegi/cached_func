CXX = g++
INCLUDEDIR=..
CXXFLAGS = -std=c++14 -Wall -I$(INCLUDEDIR)

all: basic_map basic_unordered_map

%: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f *.o basic_map basic_unordered_map


basic: $(INCLUDEDIR)/cached.hpp

#end
