CXX = g++
INCLUDEDIR=../include
CXXFLAGS = -std=c++14 -g -O3 -Wall -I$(INCLUDEDIR)

all: test_cached test_lru

%: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f *.o test_cached test_lru

check: test_cached test_lru
	./test_cached --report_level=short
	./test_lru --report_level=short

memcheck: test_cached test_lru
	valgrind --leak-check=full ./test_cached --report_level=short
	valgrind --leak-check=full ./test_lru --report_level=short

#dependencies

test_cached: $(INCLUDEDIR)/cached.hpp $(INCLUDEDIR)/lru.hpp tuple_hash.hpp
test_lru: $(INCLUDEDIR)/lru.hpp

#end
