CXX = g++
CXXFLAGS = -std=c++14 -Wall

all: test_cached

%: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

check: test_cached
	./test_cached --report_level=short
