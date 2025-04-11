CXX = g++
CXXFLAGS = -std=c++14 -Wall -Wextra -pedantic -O2
GTEST_FLAGS = -lgtest -lgtest_main -pthread

.PHONY: all clean test

all: ascii85

ascii85: ascii85.o
	$(CXX) $(CXXFLAGS) -o $@ $^

ascii85.o: ascii85.cpp ascii85.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

test: ascii85_test
	./ascii85_test

ascii85_test: ascii85_lib.o ascii85_test.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(GTEST_FLAGS)

ascii85_lib.o: ascii85.cpp ascii85.h
	$(CXX) $(CXXFLAGS) -DASCII85_LIB -c -o $@ $<

ascii85_test.o: ascii85_test.cpp ascii85.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f ascii85 ascii85_test *.o