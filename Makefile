CXX = g++
CXXFLAGS = -std=c++14 -Wall

all: test_cached

%: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f *.o test_cached

check: test_cached
	./test_cached --report_level=short

memcheck: test_cached
	valgrind --leak-check=full ./test_cached --report_level=short

test_cached: cached.h

