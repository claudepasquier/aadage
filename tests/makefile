CC = g++
CXXFLAGS = -Wall -Wextra -O2 -I../src -I../include/bliss-0.72

TESTS = test_canonical

all: $(TESTS)

test: all
	./test_canonical

test_canonical: test_canonical.cpp ../src/canonical.cpp ../src/canonical.h
	$(CC) $(CXXFLAGS) test_canonical.cpp ../src/canonical.cpp -o test_canonical

clean:
	rm -f $(TESTS)

.PHONY: all test clean
