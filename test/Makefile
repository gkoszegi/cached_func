CXX = g++
INCLUDEDIR=..
CXXFLAGS = -std=c++14 -Wall -I$(INCLUDEDIR)

all: test_cached

%: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f *.o test_cached

check: test_cached
	./test_cached --report_level=short

memcheck: test_cached
	valgrind --leak-check=full ./test_cached --report_level=short

test_cached: $(INCLUDEDIR)/cached.hpp

#end
